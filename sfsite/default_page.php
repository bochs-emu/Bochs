<?php
// Default Web Page for groups that haven't setup their page yet
// Please replace this file with your own website
//
// $Id: default_page.php,v 1.1 2001-06-17 04:14:28 bdenney Exp $
//
$headers = getallheaders();
?>
<HTML>
<HEAD>
<TITLE>SourceForge: Welcome</TITLE>
<LINK rel="stylesheet" href="http://sourceforge.net/sourceforge.css" type="text/css">
</HEAD>

<BODY bgcolor=#FFFFFF topmargin="0" bottommargin="0" leftmargin="0" rightmargin="0" marginheight="0" marginwidth="0">

<!-- top strip -->
<TABLE width="100%" border=0 cellspacing=0 cellpadding=2 bgcolor="737b9c">
  <TR>
    <TD><SPAN class=maintitlebar>&nbsp;&nbsp;
      <A class=maintitlebar href="http://sourceforge.net/"><B>Home</B></A> | 
      <A class=maintitlebar href="http://sourceforge.net/about.php"><B>About</B></A> | 
      <A class=maintitlebar href="http://sourceforge.net/partners.php"><B>Partners</B></a> |
      <A class=maintitlebar href="http://sourceforge.net/contact.php"><B>Contact Us</B></A></SPAN></TD>
    </TD>
  </TR>
</TABLE>
<!-- end top strip -->

<!-- top title table -->
<TABLE width="100%" border=0 cellspacing=0 cellpadding=0 bgcolor="" valign="center">
  <TR valign="top" bgcolor="#eeeef8">
    <TD>
      <A href="http://sourceforge.net/"><IMG src="http://sourceforge.net/images/sflogo2-steel.png" vspace="0" border=0 width="143" height="70"></A>
    </TD>
    <TD width="99%"><!-- right of logo -->
      <a href="http://www.valinux.com"><IMG src="http://sourceforge.net/images/valogo3.png" align="right" alt="VA Linux Systems" hspace="5" vspace="0" border=0 width="117" height="70"></A>
    </TD><!-- right of logo -->
  </TR>
  <TR><TD bgcolor="#543a48" colspan=2><IMG src="http://sourceforge.net/images/blank.gif" height=2 vspace=0></TD></TR>
</TABLE>
<!-- end top title table -->

<!-- center table -->
<TABLE width="100%" border="0" cellspacing="0" cellpadding="2" bgcolor="#FFFFFF" align="center">
  <TR>
    <TD>
      <CENTER><BR>
      <H1>Welcome to http://<?php print $headers[Host]; ?>/</H1>
      <P>We're Sorry but this Project hasn't yet uploaded their personal webpage yet.<BR>
      Please check back soon for updates or visit <A href="http://sourceforge.net/">SourceForge</A></P><BR>
      </CENTER>
    </TD>
  </TR>
</TABLE>
<!-- end center table -->

<!-- footer table -->
<TABLE width="100%" border="0" cellspacing="0" cellpadding="2" bgcolor="737b9c">
  <TR>
    <TD align="center"><FONT color="#ffffff"><SPAN class="titlebar">
      All trademarks and copyrights on this page are properties of their respective owners. Forum comments are owned by the poster. The rest is copyright ©1999-2000 VA Linux Systems, Inc.</SPAN></FONT>
    </TD>
  </TR>
</TABLE>

<!-- end footer table -->
</BODY>
</HTML>
