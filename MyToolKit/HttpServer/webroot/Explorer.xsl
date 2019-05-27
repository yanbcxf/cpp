<?xml version="1.0" encoding="utf-8"?><!DOCTYPE xsl:stylesheet  [
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
<xsl:output method="html" encoding="utf-8" doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN" doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"/>
<xsl:template match="/">

<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<title>Untitled Document</title>
  <link rel="stylesheet" type="text/css" href="tree_component.css" />
	<link rel="stylesheet" type="text/css" href="style.css" />
	
	<script language="javascript" type="text/javascript" src="jquery.js"></script>

<script type="text/javascript" src="_all.js"></script>

  <script type="text/javascript" src="tree_component.js"></script>
  
  
   <script type="text/javascript" src="jquery.getUrlParam.js"></script>
   
   <script type="text/javascript" src="Explorer.js"></script>
  
</head>
<body scroll="no">
<div class="demo" id="demo"></div>

  <p align="right">&nbsp;
  <input name="BtnOk" type="button" id="BtnOk" value=" 确定 " />
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
  <input name="submit" type="submit" id="Login" onclick="self.parent.tb_remove();" value=" 取消 " /> 
 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; </p>
</body>
</html>

</xsl:template>
</xsl:stylesheet>