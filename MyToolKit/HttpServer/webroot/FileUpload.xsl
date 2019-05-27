<?xml version="1.0" encoding="gb2312"?><!DOCTYPE xsl:stylesheet  [
	<!ENTITY nbsp   "&#160;">
	<!ENTITY copy   "&#169;">
	<!ENTITY reg    "&#174;">
	<!ENTITY trade  "&#8482;">
	<!ENTITY mdash  "&#8212;">
	<!ENTITY ldquo  "&#8220;">
	<!ENTITY rdquo  "&#8221;"> 
	<!ENTITY pound  "&#163;">
	<!ENTITY yen    "&#165;">
	<!ENTITY euro   "&#8364;">
]>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="gb2312" doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN" doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"/>
<xsl:template match="/">

<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>Untitled Document</title>
</head>

<body>
<p>file uploaded ：</p>
<table width="100%" border="1">
      <tbody>
        
        <tr>
          <th width="24%">file name</th>
          <th width="32%">size</th>
          <th width="44%"><div align="center">download</div>            </th>
        </tr>
		  
		<xsl:for-each select="/FileUpload/Voice">  
		
        <tr>
          <td>
            <div align="center"><xsl:value-of select="Name"/></div></td>
          <td align="right"><div align="center"><xsl:value-of select="Size"/></div></td>
          <td align="center">
		  
		  <a title="download">
            <xsl:attribute name="href">upload/<xsl:value-of select="Name"/></xsl:attribute>
			download		  </a>						 
	
		  <a title="收听">
          <xsl:attribute name="href">?Option=Listen&amp;Name=<xsl:value-of select="Name"/></xsl:attribute>
		  </a>			   </td>
          </tr>
		</xsl:for-each>
      </tbody>
    </table>

   <p>&nbsp;</p>
   <p>upload：</p>
   <form action="?Option=Upload" method="post" enctype="multipart/form-data" name="Upload">
   
    <input name="Upload" type="file" id="Upload" size="30" />        
	&nbsp;&nbsp; 
	<input type="submit" name="Submit" value="submit"/>

   
   </form>
</body>
</html>

</xsl:template>
</xsl:stylesheet>