// Uri encode and decode.
// RFC1630, RFC1738, RFC2396


#include <string>
#include <assert.h>

namespace ZvLibs
{
	std::string UriDecode(const std::string & sSrc);
	std::string UriEncode(const std::string & sSrc);
}