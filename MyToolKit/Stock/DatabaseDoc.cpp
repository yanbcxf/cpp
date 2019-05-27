// DatabaseDoc.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "DatabaseDoc.h"
#include "UrlUtils.h"
#include "CalculateStockInfoEx.h"

#include <json/json.h>

#include "HtmlParser.h"

#include "CfiBonus.h"
#include "CfiPlacementOfShare.h"
#include "BonusOf10jqka.h"
#include "CfiAddIssuance.h"
#include "SohuIndexDaily.h"
#include "CninfoStockInfo.h"


//////////////////////////////////////////////////////////////////////////

#include ".\TreeContainer408\tree.h"
#include ".\TreeContainer408\multitree.h"
#include ".\TreeContainer408\unique_tree.h"
#include ".\TreeContainer408\sequential_tree.h"

class CMyClass
{
public:
	CMyClass() : key(0) {} // needs default constructor
	CMyClass(int key_, const std::string& text_) 
		: key(key_), text(text_) {}
	const std::string& get_text() const { return text; }
	friend bool operator <(const CMyClass& lhs, const CMyClass& rhs)
	{
		return lhs.key < rhs.key;
	}

private:
	int key;
	std::string text;
};

struct CAltKey
{
	bool operator()(const CMyClass& lhs, const CMyClass& rhs)
	{
		return lhs.get_text() < rhs.get_text();
	}
};



//////////////////////////////////////////////////////////////////////////



std::string connectString;
backend_factory const &backEnd = *soci::factory_mysql();

//////////////////////////////////////////////////////////////////////////

class CXHtmlParser: public CHtmlParser
{
public:
	CXHtmlParser(tree<HTML::Node> &dom):CHtmlParser(dom)
	{

	}

	void HtmlNodeCallBack(string xpath, string val, map<string,string> attrsOfParent)
	{
		stringstream ss;
		ss << xpath << " <------> " << val;
		LOG4CPLUS_DEBUG(g_logger, ss.str());
	}
};


//////////////////////////////////////////////////////////////////////////



// procedure call test
void test1()
{
	stringstream ss;
	{
		//session sql(backEnd, connectString);
		session sql(g_MysqlPool);

		mysql_session_backend *sessionBackEnd
			= static_cast<mysql_session_backend *>(sql.get_backend());
		std::string version = mysql_get_server_info(sessionBackEnd->conn_);
		int v;
		std::istringstream iss(version);
		if ((iss >> v) and v < 5)
		{
			
			sendToOutput("skipping test 1 (MySQL server version ");
			ss << version << " does not support stored procedures)\n";
			sendToOutput(ss.str().c_str());
			return;
		}

		try { sql << "drop function myecho"; }
		catch (soci_error const &) {}

		sql <<
			"create function myecho(msg text) "
			"returns text deterministic "
			"  return msg; ";

		std::string in("my message");
		std::string out;

		statement st = (sql.prepare <<
			"select myecho(:input)",
			into(out),
			use(in, "input"));

		st.execute(1);
		assert(out == in);

		// explicit procedure syntax
		{
			std::string in("my message2");
			std::string out;

			procedure proc = (sql.prepare <<
				"myecho(:input)",
				into(out), use(in, "input"));

			proc.execute(1);
			assert(out == in);
		}

		sql << "drop function myecho";
	}

	ss << "test 1 passed" << std::endl;
	sendToOutput(ss.str().c_str());

}

// MySQL error reporting test.
void test2()
{
	{
		try
		{
			session sql(backEnd, "host=test.soci.invalid");
		}
		catch (mysql_soci_error const &e)
		{
			assert(e.err_num_ == CR_UNKNOWN_HOST ||
				e.err_num_ == CR_CONN_HOST_ERROR);
		}
	}

	{
		session sql(backEnd, connectString);
		sql << "create table soci_test (id integer)";
		try
		{
			int n;
			sql << "select id from soci_test_nosuchtable", into(n);
		}
		catch (mysql_soci_error const &e)
		{
			assert(e.err_num_ == ER_NO_SUCH_TABLE);
		}
		try
		{
			sql << "insert into soci_test (invalid) values (256)";
		}
		catch (mysql_soci_error const &e)
		{
			assert(e.err_num_ == ER_BAD_FIELD_ERROR);
		}
		// A bulk operation.
		try
		{
			std::vector<int> v(3, 5);
			sql << "insert into soci_test_nosuchtable values (:n)", use(v);
		}
		catch (mysql_soci_error const &e)
		{
			assert(e.err_num_ == ER_NO_SUCH_TABLE);
		}
		sql << "drop table soci_test";
	}

	stringstream ss;
	ss << "test 2 passed" << std::endl;
	sendToOutput(ss.str().c_str());

}

struct bigint_table_creator : table_creator_base
{
	bigint_table_creator(session & sql)
		: table_creator_base(sql)
	{
		sql << "create table soci_test(val bigint)";
	}
};

struct bigint_unsigned_table_creator : table_creator_base
{
	bigint_unsigned_table_creator(session & sql)
		: table_creator_base(sql)
	{
		sql << "create table soci_test(val bigint unsigned)";
	}
};

// long long test
void test3()
{
	{
		session sql(backEnd, connectString);

		bigint_table_creator tableCreator(sql);

		long long v1 = 1000000000000LL;
		assert(v1 / 1000000 == 1000000);

		sql << "insert into soci_test(val) values(:val)", use(v1);

		long long v2 = 0LL;
		sql << "select val from soci_test", into(v2);

		assert(v2 == v1);
	}

	// vector<long long>
	{
		session sql(backEnd, connectString);

		bigint_table_creator tableCreator(sql);

		std::vector<long long> v1;
		v1.push_back(1000000000000LL);
		v1.push_back(1000000000001LL);
		v1.push_back(1000000000002LL);
		v1.push_back(1000000000003LL);
		v1.push_back(1000000000004LL);

		sql << "insert into soci_test(val) values(:val)", use(v1);

		std::vector<long long> v2(10);
		sql << "select val from soci_test order by val desc", into(v2);

		assert(v2.size() == 5);
		assert(v2[0] == 1000000000004LL);
		assert(v2[1] == 1000000000003LL);
		assert(v2[2] == 1000000000002LL);
		assert(v2[3] == 1000000000001LL);
		assert(v2[4] == 1000000000000LL);
	}

	{
		session sql(backEnd, connectString);

		bigint_unsigned_table_creator tableCreator(sql);

		sql << "insert into soci_test set val = 18446744073709551615";
		row v;
		sql << "select * from soci_test", into(v);
	}

	{
		session sql(backEnd, connectString);

		bigint_unsigned_table_creator tableCreator(sql);

		const char* source = "18446744073709551615";
		sql << "insert into soci_test set val = " << source;
		unsigned long long vv = 0;
		sql << "select val from soci_test", into(vv);
		std::stringstream buf;
		buf << vv;
		assert(buf.str() == source);
	}

	{
		session sql(backEnd, connectString);

		bigint_unsigned_table_creator tableCreator(sql);

		const char* source = "18446744073709551615";
		sql << "insert into soci_test set val = " << source;
		std::vector<unsigned long long> v(1);
		sql << "select val from soci_test", into(v);
		std::stringstream buf;
		buf << v.at(0);
		assert(buf.str() == source);
	}

	{
		session sql(backEnd, connectString);

		bigint_unsigned_table_creator tableCreator(sql);

		unsigned long long n = 18446744073709551615ULL;
		sql << "insert into soci_test(val) values (:n)", use(n);
		unsigned long long m = 0;
		sql << "select val from soci_test", into(m);
		assert(n == m);
	}

	{
		session sql(backEnd, connectString);

		bigint_unsigned_table_creator tableCreator(sql);

		std::vector<unsigned long long> v1;
		v1.push_back(18446744073709551615ULL);
		v1.push_back(18446744073709551614ULL);
		v1.push_back(18446744073709551613ULL);
		sql << "insert into soci_test(val) values(:val)", use(v1);

		std::vector<unsigned long long> v2(10);
		sql << "select val from soci_test order by val", into(v2);

		assert(v2.size() == 3);
		assert(v2[0] == 18446744073709551613ULL);
		assert(v2[1] == 18446744073709551614ULL);
		assert(v2[2] == 18446744073709551615ULL);
	}

	stringstream ss;
	ss << "test 3 passed" << std::endl;
	sendToOutput(ss.str().c_str());
}

template <typename T>
void test_num(const char* s, bool valid, T value)
{
	stringstream ss;
	try
	{
		session sql(backEnd, connectString);
		T val;
		sql << "select \'" << s << "\'", into(val);
		if (valid)
		{
			double v1 = static_cast<double>(value);
			double v2 = static_cast<double>(val);
			double d = std::fabs(v1 - v2);
			double epsilon = 0.001;
			assert(d < epsilon ||
				d < epsilon * (std::fabs(v1) + std::fabs(v2)));
		}
		else
		{
			ss << "string \"" << s << "\" parsed as " << val
				<< " but should have failed.\n";
			sendToOutput(ss.str().c_str());
			assert(false);
		}
	}
	catch (soci_error const& e)
	{
		if (valid)
		{
			ss << "couldn't parse number: \"" << s << "\"\n";
			sendToOutput(ss.str().c_str());
			assert(false);
		}
		else
		{
			assert(std::string(e.what()) == "Cannot convert data.");
		}
	}
}

// Number conversion test.
void test4()
{
	test_num<double>("", false, 0);
	test_num<double>("foo", false, 0);
	test_num<double>("1", true, 1);
	test_num<double>("12", true, 12);
	test_num<double>("123", true, 123);
	test_num<double>("12345", true, 12345);
	test_num<double>("12341234123412341234123412341234123412341234123412341",
		true, 1.23412e+52);
	test_num<double>("99999999999999999999999912222222222222222222222222223"
		"9999999999999999999999991222222222222222222222222222333333333333"
		"9999999999999999999999991222222222222222222222222222333333333333"
		"9999999999999999999999991222222222222222222222222222333333333333"
		"9999999999999999999999991222222222222222222222222222333333333333"
		"9999999999999999999999991222222222222222222222222222333333333333"
		"9999999999999999999999991222222222222222222222222222333333333333"
		"9999999999999999999999991222222222222222222222222222333333333333"
		"9999999999999999999999991222222222222222222222222222333333333333"
		"9999999999999999999999991222222222222222222222222222333333333333"
		"9999999999999999999999991222222222222222222222222222333333333333"
		"9999999999999999999999991222222222222222222222222222333333333333"
		"9999999999999999999999991222222222222222222222222222333333333333"
		"9999999999999999999999991222222222222222222222222222333333333333"
		"9999999999999999999999991222222222222222222222222222333333333333",
		false, 0);
	test_num<double>("1e3", true, 1000);
	test_num<double>("1.2", true, 1.2);
	test_num<double>("1.2345e2", true, 123.45);
	test_num<double>("1 ", false, 0);
	test_num<double>("     123", true, 123);
	test_num<double>("1,2", false, 0);
	test_num<double>("123abc", false, 0);
	test_num<double>("-0", true, 0);

	test_num<short>("123", true, 123);
	test_num<short>("100000", false, 0);

	test_num<int>("123", true, 123);
	test_num<int>("2147483647", true, 2147483647);
	test_num<int>("2147483647a", false, 0);
	test_num<int>("2147483648", false, 0);
	// -2147483648 causes a warning because it is interpreted as
	// 2147483648 (which doesn't fit in an integer) to which a negation
	// is applied.
	test_num<int>("-2147483648", true, -2147483647 - 1);
	test_num<int>("-2147483649", false, 0);
	test_num<int>("-0", true, 0);
	test_num<int>("1.1", false, 0);

	test_num<long long>("123", true, 123);
	test_num<long long>("9223372036854775807", true, 9223372036854775807LL);
	test_num<long long>("9223372036854775808", false, 0);

	stringstream ss;
	ss << "test 4 passed";
	sendToOutput(ss.str().c_str());
}

void test5()
{
	session sql(backEnd, connectString);
	std::tm t;
	sql << "select maketime(19, 54, 52)", into(t);
	assert(t.tm_year == 100);
	assert(t.tm_mon == 0);
	assert(t.tm_mday == 1);
	assert(t.tm_hour == 19);
	assert(t.tm_min == 54);
	assert(t.tm_sec == 52);

	stringstream ss;
	ss << "test 5 passed";
	sendToOutput(ss.str().c_str());
}

// TEXT and BLOB types support test.
void test6()
{
	session sql(backEnd, connectString);
	std::string a("asdfg\0hjkl", 10);
	std::string b("lkjhg\0fd\0\0sa\0", 13);
	std::string c("\\0aa\\0bb\\0cc\\0", 10);
	// The maximum length for TEXT and BLOB is 65536.
	std::string x(60000, 'X');
	std::string y(60000, 'Y');
	// The default max_allowed_packet value for a MySQL server is 1M,
	// so let's limit ourselves to 800k, even though the maximum length
	// for LONGBLOB is 4G.
	std::string z(800000, 'Z');

	sql << "create table soci_test (id int, text_value text, "
		"blob_value blob, longblob_value longblob)";
	sql << "insert into soci_test values (1, \'foo\', \'bar\', \'baz\')";
	sql << "insert into soci_test "
		<< "values (2, \'qwerty\\0uiop\', \'zxcv\\0bnm\', "
		<< "\'qwerty\\0uiop\\0zxcvbnm\\0\')";
	sql << "insert into soci_test values (3, :a, :b, :c)",
		use(a), use(b), use(c);
	sql << "insert into soci_test values (4, :x, :y, :z)",
		use(x), use(y), use(z);

	std::vector<std::string> text_vec(100);
	std::vector<std::string> blob_vec(100);
	std::vector<std::string> longblob_vec(100);
	sql << "select text_value, blob_value, longblob_value "
		<< "from soci_test order by id",
		into(text_vec), into(blob_vec), into(longblob_vec);
	assert(text_vec.size() == 4);
	assert(blob_vec.size() == 4);
	assert(longblob_vec.size() == 4);
	assert(text_vec[0] == "foo");
	assert(blob_vec[0] == "bar");
	assert(longblob_vec[0] == "baz");
	assert(text_vec[1] == std::string("qwerty\0uiop", 11));
	assert(blob_vec[1] == std::string("zxcv\0bnm", 8));
	assert(longblob_vec[1] == std::string("qwerty\0uiop\0zxcvbnm\0", 20));
	assert(text_vec[2] == a);
	assert(blob_vec[2] == b);
	assert(longblob_vec[2] == c);
	assert(text_vec[3] == x);
	assert(blob_vec[3] == y);
	assert(longblob_vec[3] == z);

	std::string text, blob, longblob;
	sql << "select text_value, blob_value, longblob_value "
		<< "from soci_test where id = 1",
		into(text), into(blob), into(longblob);
	assert(text == "foo");
	assert(blob == "bar");
	assert(longblob == "baz");
	sql << "select text_value, blob_value, longblob_value "
		<< "from soci_test where id = 2",
		into(text), into(blob), into(longblob);
	assert(text == std::string("qwerty\0uiop", 11));
	assert(blob == std::string("zxcv\0bnm", 8));
	assert(longblob == std::string("qwerty\0uiop\0zxcvbnm\0", 20));
	sql << "select text_value, blob_value, longblob_value "
		<< "from soci_test where id = 3",
		into(text), into(blob), into(longblob);
	assert(text == a);
	assert(blob == b);
	assert(longblob == c);
	sql << "select text_value, blob_value, longblob_value "
		<< "from soci_test where id = 4",
		into(text), into(blob), into(longblob);
	assert(text == x);
	assert(blob == y);
	assert(longblob == z);

	rowset<row> rs =
		(sql.prepare << "select text_value, blob_value, longblob_value "
		"from soci_test order by id");
	rowset<row>::const_iterator r = rs.begin();
	assert(r->get_properties(0).get_data_type() == dt_string);
	assert(r->get<std::string>(0) == "foo");
	assert(r->get_properties(1).get_data_type() == dt_string);
	assert(r->get<std::string>(1) == "bar");
	assert(r->get_properties(2).get_data_type() == dt_string);
	assert(r->get<std::string>(2) == "baz");
	++r;
	assert(r->get_properties(0).get_data_type() == dt_string);
	assert(r->get<std::string>(0) == std::string("qwerty\0uiop", 11));
	assert(r->get_properties(1).get_data_type() == dt_string);
	assert(r->get<std::string>(1) == std::string("zxcv\0bnm", 8));
	assert(r->get_properties(2).get_data_type() == dt_string);
	assert(r->get<std::string>(2) ==
		std::string("qwerty\0uiop\0zxcvbnm\0", 20));
	++r;
	assert(r->get_properties(0).get_data_type() == dt_string);
	assert(r->get<std::string>(0) == a);
	assert(r->get_properties(1).get_data_type() == dt_string);
	assert(r->get<std::string>(1) == b);
	assert(r->get_properties(2).get_data_type() == dt_string);
	assert(r->get<std::string>(2) == c);
	++r;
	assert(r->get_properties(0).get_data_type() == dt_string);
	assert(r->get<std::string>(0) == x);
	assert(r->get_properties(1).get_data_type() == dt_string);
	assert(r->get<std::string>(1) == y);
	assert(r->get_properties(2).get_data_type() == dt_string);
	assert(r->get<std::string>(2) == z);
	++r;
	assert(r == rs.end());

	sql << "drop table soci_test";

	stringstream ss;
	ss << "test 6 passed";
	sendToOutput(ss.str().c_str());
}

// test for number of affected rows

struct integer_value_table_creator : table_creator_base
{
	integer_value_table_creator(session & sql)
		: table_creator_base(sql)
	{
		sql << "create table soci_test(val integer)";
	}
};

void test7()
{
	{
		session sql(backEnd, connectString);

		integer_value_table_creator tableCreator(sql);

		for (int i = 0; i != 10; i++)
		{
			sql << "insert into soci_test(val) values(:val)", use(i);
		}

		statement st1 = (sql.prepare <<
			"update soci_test set val = val + 1");
		st1.execute(false);

		assert(st1.get_affected_rows() == 10);

		statement st2 = (sql.prepare <<
			"delete from soci_test where val <= 5");
		st2.execute(false);

		assert(st2.get_affected_rows() == 5);
	}

	stringstream ss;
	ss << "test 7 passed";
	sendToOutput(ss.str().c_str());
}


// The prepared statements should survive session::reconnect().
void test8()
{
	{
		session sql(backEnd, connectString);

		integer_value_table_creator tableCreator(sql);

		int i;
		statement st = (sql.prepare
			<< "insert into soci_test(val) values(:val)", use(i));
		i = 5;
		st.execute(true);

		sql.reconnect();

		i = 6;
		st.execute(true);

		sql.close();
		sql.reconnect();

		i = 7;
		st.execute(true);

		std::vector<int> v(5);
		sql << "select val from soci_test order by val", into(v);
		assert(v.size() == 3);
		assert(v[0] == 5);
		assert(v[1] == 6);
		assert(v[2] == 7);
	}

	stringstream ss;
	ss << "test 8 passed";
	sendToOutput(ss.str().c_str());
}

struct unsigned_value_table_creator : table_creator_base
{
	unsigned_value_table_creator(session & sql)
		: table_creator_base(sql)
	{
		sql << "create table soci_test(val int unsigned)";
	}
};

// rowset<> should be able to take INT UNSIGNED.
void test9()
{
	{
		session sql(backEnd, connectString);

		unsigned_value_table_creator tableCreator(sql);

		unsigned int mask = 0xffffff00;
		sql << "insert into soci_test set val = " << mask;
		soci::rowset<> rows(sql.prepare << "select val from soci_test");
		int cnt = 0;
		for (soci::rowset<>::iterator it = rows.begin(), end = rows.end();
			it != end; ++it)
		{
			cnt++;
		}
		assert(cnt == 1);
	}

	stringstream ss;
	ss << "test 9 passed";
	sendToOutput(ss.str().c_str());
}

void test10()
{
	session sql(backEnd, connectString);

	row r;

	sql << "set @day = '5'";
	sql << "set @mm = 'december'";
	sql << "set @year = '2012'"; 
	sql << "select concat(@day,' ',@mm,' ',@year)", into(r);

	stringstream ss;
	ss << "test 10 passed";
	sendToOutput(ss.str().c_str());
}

struct double_value_table_creator : table_creator_base
{
	double_value_table_creator(session & sql)
		: table_creator_base(sql)
	{
		sql << "create table soci_test(val double)";
	}
};

void test11()
{
	const std::string expectedError =
		"Use element used with infinity or NaN, which are "   
		"not supported by the MySQL server.";
	{
		session sql(backEnd, connectString);

		double x = std::numeric_limits<double>::quiet_NaN();
		statement st = (sql.prepare << "SELECT :x", use(x, "x"));
		try {
			st.execute(true);
		} catch (soci_error const &e) {
			if (e.what() != expectedError) {
				throw;
			}
		}
	}
	{
		session sql(backEnd, connectString);

		double x = std::numeric_limits<double>::infinity();
		statement st = (sql.prepare << "SELECT :x", use(x, "x"));
		try {
			st.execute(true);
		} catch (soci_error const &e) {
			if (e.what() != expectedError) {
				throw;
			}
		}
	}
	{
		session sql(backEnd, connectString);
		double_value_table_creator tableCreator(sql);

		std::vector<double> v(1, std::numeric_limits<double>::quiet_NaN());
		try {
			sql << "insert into soci_test (val) values (:val)", use(v);
		} catch (soci_error const &e) {
			if (e.what() != expectedError) {                                      
				throw;
			}
		}
	}
	{
		session sql(backEnd, connectString);
		double_value_table_creator tableCreator(sql);

		std::vector<double> v(1, std::numeric_limits<double>::infinity());
		try {
			sql << "insert into soci_test (val) values (:val)", use(v);
		} catch (soci_error const &e) {
			if (e.what() != expectedError) {                                      
				throw;
			}
		}
	}

	stringstream ss;
	ss << "test 11 passed";
	sendToOutput(ss.str().c_str());
}

struct tinyint_value_table_creator : table_creator_base
{
	tinyint_value_table_creator(session & sql)
		: table_creator_base(sql)
	{
		sql << "create table soci_test(val tinyint)";
	}
};

struct tinyint_unsigned_value_table_creator : table_creator_base
{
	tinyint_unsigned_value_table_creator(session & sql)
		: table_creator_base(sql)
	{
		sql << "create table soci_test(val tinyint unsigned)";
	}
};

void test12()
{
	{
		session sql(backEnd, connectString);
		unsigned_value_table_creator tableCreator(sql);
		unsigned int mask = 0xffffff00;
		sql << "insert into soci_test set val = " << mask;
		row r;
		sql << "select val from soci_test", into(r);
		assert(r.size() == 1);
		assert(r.get_properties("val").get_data_type() == dt_long_long);
		assert(r.get<long long>("val") == 0xffffff00);
		assert(r.get<unsigned>("val") == 0xffffff00);
	}
	{
		session sql(backEnd, connectString);
		tinyint_value_table_creator tableCreator(sql);
		sql << "insert into soci_test set val = -123";
		row r;
		sql << "select val from soci_test", into(r);
		assert(r.size() == 1);
		assert(r.get_properties("val").get_data_type() == dt_integer);
		assert(r.get<int>("val") == -123);
	}
	{
		session sql(backEnd, connectString);
		tinyint_unsigned_value_table_creator tableCreator(sql);
		sql << "insert into soci_test set val = 123";
		row r;
		sql << "select val from soci_test", into(r);
		assert(r.size() == 1);
		assert(r.get_properties("val").get_data_type() == dt_integer);
		assert(r.get<int>("val") == 123);
	}
	{
		session sql(backEnd, connectString);
		bigint_unsigned_table_creator tableCreator(sql);
		sql << "insert into soci_test set val = 123456789012345";
		row r;
		sql << "select val from soci_test", into(r);
		assert(r.size() == 1);
		assert(r.get_properties("val").get_data_type() == dt_unsigned_long_long);
		assert(r.get<unsigned long long>("val") == 123456789012345ULL);
	}
	{
		session sql(backEnd, connectString);
		bigint_table_creator tableCreator(sql);
		sql << "insert into soci_test set val = -123456789012345";
		row r;
		sql << "select val from soci_test", into(r);
		assert(r.size() == 1);
		assert(r.get_properties("val").get_data_type() == dt_long_long);
		assert(r.get<long long>("val") == -123456789012345LL);
	}

	stringstream ss;
	ss << "test 12 passed";
	sendToOutput(ss.str().c_str());
}

struct strings_table_creator : table_creator_base
{
	strings_table_creator(session & sql)
		: table_creator_base(sql)
	{
		sql << "create table soci_test(s1 char(20), s2 varchar(20), "
			"s3 tinytext, s4 mediumtext, s5 text, s6 longtext, "
			"b1 binary(20), b2 varbinary(20), b3 tinyblob, b4 mediumblob, "
			"b5 blob, b6 longblob, e1 enum ('foo', 'bar', 'baz'))";
	}
};

void test13()
{
	{
		session sql(backEnd, connectString);
		strings_table_creator tableCreator(sql);
		std::string text = "Ala ma kota.";
		std::string binary("Ala\0ma\0kota.........", 20);
		sql << "insert into soci_test "
			"(s1, s2, s3, s4, s5, s6, b1, b2, b3, b4, b5, b6, e1) values "
			"(:s1, :s2, :s3, :s4, :d5, :s6, :b1, :b2, :b3, :b4, :b5, :b6, "
			"\'foo\')",
			use(text), use(text), use(text), use(text), use(text), use(text),
			use(binary), use(binary), use(binary), use(binary), use(binary),
			use(binary);
		row r;
		sql << "select s1, s2, s3, s4, s5, s6, b1, b2, b3, b4, b5, b6, e1 "
			"from soci_test", into(r);
		assert(r.size() == 13);
		for (int i = 0; i < 13; i++) {
			assert(r.get_properties(i).get_data_type() == dt_string);
			if (i < 6) {
				assert(r.get<std::string>(i) == text);
			} else if (i < 12) {
				assert(r.get<std::string>(i) == binary);
			} else {
				assert(r.get<std::string>(i) == "foo");
			}
		}
	}

	stringstream ss;
	ss << "test 13 passed";
	sendToOutput(ss.str().c_str());
}

// DDL Creation objects for common tests
struct table_creator_one : public table_creator_base
{
	table_creator_one(session & sql)
		: table_creator_base(sql)
	{
		sql << "create table soci_test(id integer, val integer, c char, "
			"str varchar(20), sh int2, ul numeric(20), d float8, "
			"tm datetime, i1 integer, i2 integer, i3 integer, "
			"name varchar(20)) engine=InnoDB";
	}
};

struct table_creator_two : public table_creator_base
{
	table_creator_two(session & sql)
		: table_creator_base(sql)
	{
		sql  << "create table soci_test(num_float float8, num_int integer,"
			" name varchar(20), sometime datetime, chr char)";
	}
};

struct table_creator_three : public table_creator_base
{
	table_creator_three(session & sql)
		: table_creator_base(sql)
	{
		sql << "create table soci_test(name varchar(100) not null, "
			"phone varchar(15))";
	}
};

struct table_creator_for_get_affected_rows : table_creator_base
{
	table_creator_for_get_affected_rows(session & sql)
		: table_creator_base(sql)
	{
		sql << "create table soci_test(val integer)";
	}
};

//
// Support for SOCI Common Tests
//

class test_context : public test_context_base
{
public:
	test_context(backend_factory const &backEnd,
		std::string const &connectString)
		: test_context_base(backEnd, connectString) {}

	table_creator_base* table_creator_1(session& s) const
	{
		return new table_creator_one(s);
	}

	table_creator_base* table_creator_2(session& s) const
	{
		return new table_creator_two(s);
	}

	table_creator_base* table_creator_3(session& s) const
	{
		return new table_creator_three(s);
	}

	table_creator_base* table_creator_4(session& s) const
	{
		return new table_creator_for_get_affected_rows(s);
	}

	std::string to_date_time(std::string const &datdt_string) const
	{
		return "\'" + datdt_string + "\'";
	}

};

bool are_transactions_supported()
{
	session sql(backEnd, connectString);
	sql << "drop table if exists soci_test";
	sql << "create table soci_test (id int) engine=InnoDB";
	row r;
	sql << "show table status like \'soci_test\'", into(r);
	bool retv = (r.get<std::string>(1) == "InnoDB");
	sql << "drop table soci_test";
	return retv;
}

UINT MysqlTestThreadFunc(LPVOID param)
{
	stringstream ss;
	connectString = "dbname=stock port=3366   host=127.0.0.1 user=root password=\'\' ";

	try
	{
		test_context tc(backEnd, connectString);
		common_tests tests(tc);
		bool checkTransactions = are_transactions_supported();
		//tests.run(checkTransactions);

		ss << "\nSOCI MySQL Tests:\n\n";
		sendToOutput(ss.str().c_str());

		test1();
		test2();
		test3();
		test4();
		test5();
		test6();
		test7();
		test8();
		test9();
		test10();
		if (std::numeric_limits<double>::is_iec559) {
			test11();
		} else {
			ss.str("");
			ss << "Skipping test11 "
				<< "(C++ implementation's double type is not IEC-559)\n";
			sendToOutput(ss.str().c_str());
		}
		test12();
		test13();

		ss.str("");
		ss << "\nOK, all tests passed.\n\n";
		sendToOutput(ss.str().c_str());
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\n';
		sendToOutput(ss.str().c_str());
	}
	return 0;
}

//	创建 股票的 数据库表格
UINT StockSetupScriptThreadFunc(LPVOID param)
{

	stringstream ss;
	connectString = "dbname=stock port=3366   host=127.0.0.1 user=root  ";

	try
	{
		test_context tc(backEnd, connectString);
		
		ss << "\nSOCI 股票数据库开始创建\n\n";
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);

		//session sql(backEnd, connectString);
		session sql(g_MysqlPool);

		sql << "create table if not exists TradingDaily (Code int ,TradeDate date, Open DECIMAL(10,2), High DECIMAL(10,2), "
			"Low DECIMAL(10,2), Close DECIMAL(10,2), Volume int, Adj_Close DECIMAL(10,2),  PRIMARY KEY ( Code, TradeDate))";

		sql << "create table if not exists Bonus (Code int ,BonusDate date, BonusYear date, IncreaseStock int, "
			"remark varchar(256),  PRIMARY KEY ( Code, BonusDate))";

		sql << "create table if not exists FinanceReport (Code int ,ReportDate date, NetProfit DECIMAL(12,2),"
			"PrimeOperatingRevenue DECIMAL(12,2), TotalAssets DECIMAL(12,2), PRIMARY KEY ( Code, ReportDate))";

		sql << "create table if not exists Shareholding (Code int ,ReportDate date, TotalShare DECIMAL(12,2),"
			"CirculatingShare DECIMAL(12,2), PRIMARY KEY ( Code, ReportDate))";

		CEntrepreneurship cent;
		sql << cent.SqlScript();

		CBalance cbalance;
		sql << cbalance.SqlScript();

		CCashFlows ccf;
		sql << ccf.SqlScript();

		CIncomeStatement cis;
		sql << cis.SqlScript();

		CCapitalStructure cca;
		sql << cca.SqlScript();

		CShenzhenMarketQuotation quotation;
		string test = quotation.SqlScript();
		sql << quotation.SqlScript();

		CCalculateStockInfoEx cse(NULL, WM_USER_LOG_1);
		test = cse.SqlScript();
		sql << cse.SqlScript();

		CNetEaseTradeDaily ctd;
		test = ctd.SqlScript();
		sql << ctd.SqlScript();

		CSouhuStockPlates csp;
		test = csp.SqlScript();
		sql << csp.SqlScript();

		CCapitalFlows163 csf;
		test = csf.SqlScript();
		sql << csf.SqlScript();

		CSohuPerformanceForecast spf;
		test = spf.SqlScript();
		sql << spf.SqlScript();

		CSohuMainIncomeComposition smi;
		test = smi.SqlScript();
		sql << smi.SqlScript();

		
		CSelfSelectStockPlates ssp;
		test = ssp.SqlScript();
		sql << ssp.SqlScript();

		CSohuFundHold sfh;
		test = sfh.SqlScript();
		sql << sfh.SqlScript();

		ss.str("");
		ss << "\nOK, 所有表格创建完毕.\n\n";
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\n';
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}
	return 0;
}


//	从上海证券交易所 获取“股本结构”测试
UINT CurlTestThreadFunc(LPVOID param)
{
	//////////////////////////////////////////////////////////////////////////
	
	CCninfoStockInfo *pStockinfo = new CCninfoStockInfo;
	pStockinfo->SaveAsCsv();

	//////////////////////////////////////////////////////////////////////////
	CCninfoAnnouncement * pAnnounce = new CCninfoAnnouncement;
	pAnnounce->m_nCode = 601012;
	pAnnounce->SaveAsCsv();

	/*using namespace tcl;
	tcl::tree<CMyClass> my_tree;

	multitree<CMyClass, CAltKey> my_multi_tree(CMyClass(0, "Root"));

	unique_tree<CMyClass, std::less<CMyClass>, CAltKey> my_unique_tree;

	sequential_tree<CMyClass> my_sequential_tree;*/

	//////////////////////////////////////////////////////////////////////////
	CCsiCsrcPeDaily * pCsdc = new CCsiCsrcPeDaily;

	/*pCsdc->GetMenuTree("fsyd","zb", "zb");
	pCsdc->SaveMenuTree();*/

	/*pCsdc->m_nCode = 20160901;
	pCsdc->SaveAsCsv();
	pCsdc->ImportToDatabase();*/

	//////////////////////////////////////////////////////////////////////////

	//char tmpCode[256] = {0};
	//sprintf(tmpCode, "http://data.stats.gov.cn/login.htm?m=ajaxLogin");

	//string test = string(tmpCode);
	//
	//string  strGBK=CUrlUtils::PostUrlOfStatsGov(tmpCode, "username=yss2002%40qq.com&keyp=991105" /*"m=isLogin"*/);

	//strGBK = Utf8_GBK(strGBK);

	//string test1 = UrlDecode("GET /easyquery.htm?m=QueryData&dbcode=hgjd&rowcode=zb&colcode=sj&wds=%5B%5D&dfwds=%5B%7B%22wdcode%22%3A%22sj%22%2C%22valuecode%22%3A%221999-%22%7D%5D&k1=1471400916895");
	//
	//strGBK = CUrlUtils::PostUrlOfStatsGov("http://data.stats.gov.cn/easyquery.htm?m=QueryData&dbcode=hgjd&rowcode=zb&colcode=sj&wds=%5B%5D&dfwds=%5B%7B%22wdcode%22%3A%22zb%22%2C%22valuecode%22%3A%22A0101%22%7D%5D&k1=1471399000626", NULL);

	//strGBK = Utf8_GBK(strGBK);

	//int k = 1;

	//////////////////////////////////////////////////////////////////////////
	//	开始按照股票代码下载某日的成交明细
	/*CSinaDealDetail * pCf1 = new CSinaDealDetail;
	pCf1->m_request_trade_date = "2016-09-12";
	vector<int>	vecCode = pCf1->NeedDownload();
	delete pCf1;

	for(int k= 0; k < vecCode.size(); k++)
	{
		CSinaDealDetail * pCf = new CSinaDealDetail;
		pCf->m_request_trade_date = "2016-09-12";
		pCf->m_nCode = vecCode[k];
		string str1 =  pCf->SqlScript();
		pCf->SaveAsCsv();
		pCf->ImportToDatabase();
		delete pCf;
	}*/
	int www= 1;
	//////////////////////////////////////////////////////////////////////////
	
	//pCf->ImportToDatabase();


//	pCf->ImportToDatabase();
	
	//http://query.sse.com.cn/security/stock/commonQuery.do?jsonCallBack=jsonpCallback78750&isPagination=false&sqlId=COMMON_SSE_ZQPZ_GP_GPLB_C&productid=600007


	/*string src="1234567890abcdef789abbbbb";
	src = ReplaceString(src, "789", ",teacher,");*/

	/*CHexunResearchPaper  * pCS = new CHexunResearchPaper;
	pCS->m_nCode = 300067;
	pCS->SaveAsCsv();
	pCS->ImportToDatabase();*/


	/*char tmpCode[256] = {0};
	sprintf(tmpCode, "http://q.stock.sohu.com/cn/300067/cjmx.shtml");

	string test = string(tmpCode);

	string  strGBK=CUrlUtils::PostUrlOfSouhu(tmpCode,NULL);

	strGBK = Utf8_GBK(strGBK);*/

	//HTML::ParserDom parser;
	//parser.parse(strGBK);
	//tree<HTML::Node> dom = parser.getTree();

	//tree<HTML::Node>::iterator it = dom.begin();
	//tree<HTML::Node>::iterator end = dom.end();
	//for (; it != end; ++it)
	//{
	//	if (it->tagName() =="div")
	//	{
	//		it->parseAttributes();

	//		map<string,string> attrs = it->attributes();
	//		if(attrs.size()>0 && attrs["class"] == "BIZ_innerContent")
	//		{
	//			break;
	//		}

	//	}
	//}

	//if(it!=end)
	//{
	//	CXHtmlParser parser(dom);
	//	parser.Parse(it, "");
	//}


	return 0;
}

//////////////////////////////////////////////////////////////////////////
// CDatabaseDoc

IMPLEMENT_DYNCREATE(CDatabaseDoc, CDocument)

CDatabaseDoc::CDatabaseDoc()
{
}

BOOL CDatabaseDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CDatabaseDoc::~CDatabaseDoc()
{
}


BEGIN_MESSAGE_MAP(CDatabaseDoc, CDocument)
	ON_BN_CLICKED(IDC_MYSQL_TEST, &CDatabaseDoc::OnBnClickedMysqlTest)
	ON_BN_CLICKED(IDC_STOCK_DB_SETUP, &CDatabaseDoc::OnBnClickedStockDbSetup)
	ON_BN_CLICKED(IDC_CURL_TEST, &CDatabaseDoc::OnBnClickedCurlTest)
END_MESSAGE_MAP()


// CDatabaseDoc 诊断

#ifdef _DEBUG
void CDatabaseDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CDatabaseDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CDatabaseDoc 序列化

void CDatabaseDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}
#endif


// CDatabaseDoc 命令

void CDatabaseDoc::OnBnClickedMysqlTest()
{
	// TODO: 在此添加控件通知处理程序代码

	((CStockApp *)AfxGetApp())->StartWorkerThread(MysqlTestThreadFunc,(LPVOID)NULL);

}

void CDatabaseDoc::OnBnClickedStockDbSetup()
{
	// TODO: 在此添加控件通知处理程序代码
	((CStockApp *)AfxGetApp())->StartWorkerThread(StockSetupScriptThreadFunc,(LPVOID)NULL);
}

void CDatabaseDoc::OnBnClickedCurlTest()
{
	// TODO: 在此添加控件通知处理程序代码
	((CStockApp *)AfxGetApp())->StartWorkerThread(CurlTestThreadFunc,(LPVOID)NULL);

	//_beginthread(CurlTestThreadFunc, 0 ,0);
}
