#ifndef _MEDIA_HH
#define _MEDIA_HH

#ifndef _HASH_TABLE_HH
#include "HashTable.h"
#endif

#include "UsageEnvironment.h"

#define mediumNameMaxLen 30


class Medium
{
public:
	static Boolean lookupByName(UsageEnvironment& env,
		char const* mediumName,
		Medium*& resultMedium);

	static void close(UsageEnvironment& env, char const* mediumName);
	static void close(Medium* medium); // alternative close() method using ptrs
	// (has no effect if medium == NULL)

	virtual UsageEnvironment& envir() const {return fEnviron;}

	char const* name() const {return fMediumName;}

protected:
	friend class MediaLookupTable;
	Medium(UsageEnvironment& env); // abstract base class
	virtual ~Medium(); // instances are deleted using close() only

	UsageEnvironment& fEnviron;

private:
	char fMediumName[mediumNameMaxLen];
	//TaskToken fNextTask;
};


// A data structure for looking up a Medium by its string name.
// (It is used only to implement "Medium", but we make it visible here, in case developers want to use it to iterate over
//  the whole set of "Medium" objects that we've created.)
class MediaLookupTable {
public:
	static MediaLookupTable* ourMedia(UsageEnvironment& env);
	HashTable const& getTable() { return *fTable; }

protected:
	MediaLookupTable(UsageEnvironment& env);
	virtual ~MediaLookupTable();

private:
	friend class Medium;

	Medium* lookup(char const* name) const;
	// Returns NULL if none already exists

	void addNew(Medium* medium, char* mediumName);
	void remove(char const* name);

	void generateNewName(char* mediumName, unsigned maxLen);

private:
	UsageEnvironment& fEnv;
	HashTable* fTable;
	unsigned int fNameGenerator;
};


// The structure pointed to by the "liveMediaPriv" UsageEnvironment field:
class _Tables {
public:
	static _Tables* getOurTables(UsageEnvironment& env, Boolean createIfNotPresent = True);
	// returns a pointer to an "ourTables" structure (creating it if necessary)
	void reclaimIfPossible();
	// used to delete ourselves when we're no longer used

	MediaLookupTable* mediaTable;
	void* socketTable;

protected:
	_Tables(UsageEnvironment& env);
	virtual ~_Tables();

private:
	UsageEnvironment& fEnv;
};


#endif