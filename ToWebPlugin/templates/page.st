<html>
<head>
<style>
/* From "http://bluerobot.com/web/layouts/layout1.html" with modifications by Erik Oliver */
body {
    font         : 11px verdana, arial, helvetica, sans-serif;
    line-height  : 12px;
    margin-top   : 5px;
    margin-bottom: 5px;
    background-image: url("/tartan.gif");
    background-attachment : fixed ; /* scroll or fixed */
    background-position: top left;
    background-color: #eeeeee;
    color : white ;
    width: 900px ;
}

td,tr,th,table {
    border-color:yellow ;
}

#story table {
	background	:	navy;
	border		:	2px solid yellow;
} 

/* put a yellow border on the photos */
img {
    border : 2px solid yellow;
}

/* no border in the menu */
#menu img  {
    border : 0px ;
}

h1 {
    font-family: "Comic Sans MS", "Times New Roman Italic", Times, serif;
    margin:0px 0px 15px 0px;
    padding:0px;
    font-size:28px;
    line-height:28px;
    font-weight:900;
    color:yellow ;
    text-align: right ;
}

h2 {
    font-family: "Comic Sans MS", "Times New Roman Italic", Times, serif;
    margin: 0px 0px 15px 0px;
    padding:0px;
    font-size: 24px;
    line-height: 24px;
    font-weight:900;
    color:yellow;
}

h3 {
    font-family: "Comic Sans MS", "Times New Roman Italic", Times, serif;
    margin: 0px 0px 15px 0px;
    padding:0px;
    font-size: 20px;
    line-height: 20px;
    font-weight:900;
    color:yellow;
    margin-top   : 10px;
    margin-bottom: 10px;
}

h4 {
    font-family: "Comic Sans MS", "Times New Roman Italic", Times, serif;
    margin: 0px 0px 15px 0px;
    padding:0px;
    font-size: 16px;
    line-height: 16px;
    color:yellow;
}

p {
    font:11px verdana, arial, helvetica, sans-serif;
    line-height:13px;
    margin-top   : 5px;
}

td,tr {
    font:11px verdana, arial, helvetica, sans-serif;
    vertical-align : top ;
    margin-top   : 10px;
    margin-bottom: 10px;
}

a {
    color: white;
    font-size:11px;
    text-decoration:none;
    font-weight: bold;
    font-family:verdana, arial, helvetica, sans-serif;
}

#Menu a {
    font-weight: normal;
}

a:link {color:yellow;}
a:visited {color:yellow;}
a:hover {background-color:red; /*#eee; */}

#Header {
    position:absolute;
    top  :  20px;
    left :  100px;
    margin      : 50px 0px 10px 0px;
    padding     :   17px 0px 0px 20px;
    /* For IE5/Win's benefit height = [correct height] + [top padding] + [top and bottom border widths] */
    height      :33px;       /* 14px + 17px + 2px = 33px */
    border-style:solid  ;
    border-color:red ;
    border-width:2px    ; 
    line-height :0px    ;
    background-color: white ;

/* Here is the ugly brilliant hack that protects IE5/Win from its own stupidity. 
Thanks to Tantek Celik for the hack and to Eric Costello for publicizing it. 
IE5/Win incorrectly parses the "\"}"" value, prematurely closing the style 
declaration. The incorrect IE5/Win value is above, while the correct value is 
below. See http://glish.com/css/hacks.asp for details. */

    voice-family: "\"}\"";
    voice-family:inherit;
}

/* I've heard this called the "be nice to Opera 5" rule. Basically, it feeds correct 
length values to user agents that exhibit the parsing error exploited above yet get 
the CSS box model right and understand the CSS2 parent-child selector. ALWAYS include
a "be nice to Opera 5" rule every time you use the Tantek Celik hack (above). */

#Header a {
    color:yellow;
    font-size:17px;
    text-decoration:none;
    font-weight:normal;
    font-family:verdana, arial, helvetica, sans-serif;
    }

#Content {
    top    :  300px;
    left   :  300px;
    margin :  0px 0px 0px 210px;
    padding: 10px ;
}

li {
    font:10px verdana, arial, helvetica, sans-serif;
    position: relative;
    left :  -20px;
    padding: 0px;
    line-height:12px;
    }

#Content th {
    color: yellow ;
    font-weight: bold ;
    text-align : left ;
    vertical-align : bottom ;
}

#Content td{
    color      : yellow     ;
    text-align : left       ;
    vertical-align : bottom ;
}

#Content p{
    color      : white     ;
}

#Menu {
    position:absolute;
    top  :  10px;
    left :  20px;
    width: 172px;
 /* overflow: auto; */
    background-image : url("/california_tartan.gif") ;
    padding:10px;
    background-color: white ; /* #eee; */
    border:2px solid cyan;
    line-height:14px;
/* Again, the ugly brilliant hack. */
    voice-family: "\"}\"";
    voice-family:inherit;
    color: yellow;
    font-weight:normal;
    font-family:verdana, arial, helvetica, sans-serif;
    font-size:11px;
    }


.center { text-align: center;}

#NewsFlash {
    position:absolute;
    top     :  10px;
    left    :  630px;
    width   :  320px;
    padding :10px;
    color   :  yellow ;
    margin-left: 16mm;
    margin-right: 1.5%;
    margin  :5px;
    padding :2px;
    font-size: 12px ;
}

table.story {
	background	: navy;
	border		: 2px solid cyan;
	padding		: 4px;
	width		: 550px;
}

table.tartan {
	background-image	:	url("/california_tartan.gif");
}

.gradient {
	background: skyblue; /* for non-css3 browsers */

	filter		: progid:DXImageTransform.Microsoft.gradient(startColorstr='skyblue', endColorstr='navy'); /* for IE */
	background	: -webkit-gradient(linear, left top, left bottom, from(#00BFFF), to(navy)); /* for webkit browsers */
	background	: -moz-linear-gradient(top,  skyblue,  navy); /* for firefox 3.6+ */ 
}

.padded {
 	padding-left	: 10px;
 	padding-right	: 10px;
}
.verypadded {
 	padding-left	: 100px;
 	padding-right	: 100px;
}

/* That's all Folks! */

</style>
<title>$title$</title>
</head>

<body>
<meta name="apple-mobile-web-app-capable" content="yes" />


  <div id="Menu" style=";z-index:0;">

    <p align="center">
      <img src="/robinali.gif" border=0 style="border:none;"
        width=150 alt="Robin and Alison Mills at home, Oct 2002">
    </p>

  <h3>Recent Photos</h3>
        <a href ="/2014/MayMornings/"   			  	>May&nbsp;Mornings&nbsp;<img src="/new.gif" style="border:0px;"></a></a>
    <br><a href ="/2014/DiabetesLunch/"   			  	>Diabetes&nbsp;Lunch&nbsp;<img src="/new.gif" style="border:0px;"></a></a>
    <br><a href ="/2014/May/"   			  			>May&nbsp;Photos&nbsp;<img src="/new.gif" style="border:0px;"></a></a>
    <br><a href ="/Homepages/2014/Tombstone3.shtml"     >Spring Trip</a></a>
    <br><a href ="/Homepages/2014/NewYearsDay.shtml"    >The Holidays 2013</a>
    <br><a href ="/2013/NewDrive/"                      >New&nbsp;Drive</a>
    <br><a href ="/2013/SummerGardens/"                 >Our Garden&nbsp;</a>
    <br><a href ="/Homepages/2013/SummerTrip.shtml"     >Summer Trip</a>
  <hr>
  <h3>Articles</h3>
    <a href ="/articles/"                               >Article Index</a><br>
    <a href ="/files/FractalsInProjectPlanning.txt"     >Fractals&nbsp;in&nbsp;Project&nbsp;Planning</a><br>
    <a href ="/k3d/"                                    >3D&nbsp;JavaScript</a><br>
    <a href ="/articles/portforwarding/"                >Port Forwarding</a><br>
    <a href ="/articles/VirtualHosts/"                  >Virtual Hosts</a><br>
    <a href ="/articles/gpsexiftags/"                   >GeoTagging Article</a><br>
    <a href ="/articles/cocoatutorials/"                >Cocoa Tutorials</a><br>
    <a href ="/articles/macinstallers/"                 >Mac&nbsp;Installers</a><br>
  <hr>
  <h3>Regulars</h3>
    <a href ="/default.shtml"                           >Home</a><br>
    <a href ="/articles"                                >Articles&nbsp;</a><br>
    <a href ="/about.shtml"                             >About</a><br>
    <a href ="/ali.shtml"                               >Ali</a><br>
    <a href ="/australia.shtml"                         >Australia</a><br>
    <a href ="/cats.shtml"                              >Cats</a><br>
    <a href ="/diet.shtml"                              >Diet</a><br>
    <a href ="/duncan.shtml"                            >Duncan</a><br>
    <a href ="/defaults.shtml"                          >Home Pages&nbsp;<img src="/updated.gif" style="border:0px;"></a><br>
    <a href ="/jamie.shtml"                             >Jamie</a><br>
    <a href ="/k2"                                      >Kulas in Vietnam</a><br>
    <a href ="/links.shtml"                             >Links</a><br>
    <a href ="/states/parks.shtml"                      >National Parks&nbsp;<img src="/updated.gif" style="border:0px;"></a><br>
    <a href ="/photos.shtml"                            >Photos&nbsp;2014&nbsp;<img src="/updated.gif" style="border:0px;"></a><br>
    <a href ="/photos"                                  >Photos&nbsp;Earlier&nbsp;Years</a><br>
    <a href ="/poems.shtml"                             >Poetry</a><br>
    <a href ="/Posters"                                 >Posters</a><br>
    <a href ="/robin.shtml"                             >Robin</a><br>
    <a href ="/states/visits.shtml"                     >USA&nbsp;<img src="/updated.gif" style="border:0px;"></a><br>
    <a href ="/tartan.shtml"                            >Tartan&nbsp;&amp;&nbsp;Kilts</a><br>
    <a href ="/Tinkerbell"                              >Tinkerbell</a><br>
    <a href ="/training.shtml"                          >Training</a><br>
    <a href ="/travel.shtml"                            >Travel&nbsp;</a><br>
    <a href ="/Wallpapers/"                             >Wallpapers</a><br>
    <a href ="/weddings.shtml"                          >Weddings</a><br>
<center>

    <a href="http://www.facebook.com/clanmills" target="_TOP"><img src="http://badge.facebook.com/badge/719127922.5983.1415341347.png" style="border: 0px;" /></a><br>
    <img align="middle" src="/robinkilt.gif" border="0" alt="Robin in his kilt." style="border:none;"><br>

    <img align="middle" src="/MillsCoa.gif" border="0" alt="Mills Coat of Arms" style="border:none;">
    <p style="text-align:right;">
        <b>Web&nbsp;Master:</b><br>
        Robin&nbsp;Mills<br>
    </p>
    <a href="mailto:robin@clanmills.com"><i>robin@clanmills.com</i></a>
    <hr>
    <a href='https://www.ohloh.net/accounts/119737?ref=Rank' target='_blank'>
       <img alt='Ohloh profile for clanmills' border='0' height='24' src='http://www.ohloh.net/accounts/119737/widgets/account_rank.gif' width='32' style='border:0px solid transparent;'/>
    </a>
    &nbsp;&nbsp;&nbsp;&nbsp;
    <a href="http://whos.amung.us/show/jv5zkz8k" target="_TOP"><img style="border:0px;" src="http://whos.amung.us/widget/jv5zkz8k" alt="hit counter" width="81" height="29" border="0" /></a>

</center>
</div>

<script language="JavaScript">
function keyDown(event)
{
    if ( !event         ) event = window.event;
    if ( !event.keyCode ) return ;
    
    var c  = event.keyCode ;
    var M = 'M'.charCodeAt(0);  var Z = 'Z'.charCodeAt(0);
    
    var n = 0; // dont move
    switch ( c ) {
        case 37 : n = '/prev.gif' ; break; // <- 
        case 39 : n = '/next.gif' ; break; // ->
        case 38 : n = '/up.gif'   ; break; // ^
        case 40 : n = '/home.gif' ; break; // v
    }
    move(n)
}

function move(n) // '/prev.gif' or '/next.gif'
{
    if ( n ) {
        var a;
        var as = document.getElementsByTagName('a');
    	for ( a in as ) {
    	  try {
    		var A = as[a];
    		var I = A.childElementCount;
    		if ( I ) {
    		    var c = A.childNodes[0];
    			if ( c.src )
    			    if ( c.src.indexOf(n) > 0 )
    				    document.location=A.href;
    		}
    	  } catch (e) {}
    	} 
    	console.log(n);
    }

}

function tran(forward)
{
    move ( forward ? '/next.gif' : '/prev.gif' ) ;
}

function inRange(x,l,h)
{
	return l < x && x < h ;
}

function mouseDown(event)
{
    if ( !event ) event = window.event;
    if (  event['button'] ) return ; // only respect button 0

    if ( event.target.tagName.toLowerCase() == 'img' &&  event.target.src.match('Images') ) {
		var w = event.target.width;
		var h = event.target.height;
		var W = parseFloat(w);
		var H = parseFloat(h);
		var x = parseFloat(event.offsetX)/W;
		var y = parseFloat(event.offsetY)/H;
		var m = inRange(x,0.3,0.7) && inRange(y,0.3,0.7); // middle
		var u = inRange(x,0.3,0.7) && inRange(y,0.8,1.0); // up
		var f = inRange(x,0.5,1.0) || inRange(y,0.5,1.0); // forward/backward   
		if      ( m ) move('/up.gif'  );
		else if ( u ) move('/home.gif'); 
		else          tran(f);
	}
}
function findImage()
{
	var result=null;
	var   is = document.getElementsByTagName('img');
	for ( i in is ) {
		i=is[i];
		if ( i.src.match(/Images/) ) {
			result=i;
			break;
		}
	}
	return result;
}

function onLoad()
{
    var bIOS = false;
    try {
        bIOS = navigator.userAgent.toLowerCase().match(/(iphone|ipod)/);
    } catch(e) {}
    
	var img = bIOS ? findImage() : null;

    if ( img ) {
    	img.style.position='absolute';
    	img.style.top=10;
	}
	
}

document.body.onmousedown=mouseDown
document.body.onkeydown  =keyDown
window.onload            =onLoad

</script>

<div id = "Content">

<script type="text/javascript">
    // cursor position = x,y
    var y  =  0 ;
    var x  =  0 ;

    // which browser?
    var n4 = (document.layers) ;
    var n6 = (document.getElementById && !document.all) ;
    var ie = (document.all) ;
    var o6 = navigator.appName.indexOf("Opera") >= 0 ;

    if ( false ) {
      n4 = n4 ? true : false ;
      n6 = n6 ? true : false ;
      ie = ie ? true : false ;
      o6 = o6 ? true : false ;
      var lf = ' \n ' ;
      alert( 'n4 = ' + n4 + lf
           + 'n6 = ' + n6 + lf 
           + 'ie = ' + ie + lf
           + 'o6 = ' + o6 + lf
           ) ;
    }

    function abs(x) { return x >= 0 ? x : -x ; }

    function fixup(s)
    {
        var p  = '' ;
        var a  = "'"   ;
        var b  = "\\'" ;
        for ( var i = 0 ; i < s.length ; i++ ) {
          var  c = "" + s.substr(i,1) ;
          if ( c == a ) p += b ;
          else          p += c ;
        }
        return p ;
    }

    function mouse_nn(e)
    {
        y = e.pageY - window.pageYOffset ;
        x = e.pageX ;
        mouseMove() ;
        e.preventDefault(true) ;
        e.cancelBubble = true  ;
        return true ;
    }

    function mouse_ie()
    {
        var e  = window.event ;
        e.cancelBubble = true ;
        y = ie ? event.clientY : event.clientY - window.pageYOffset ;
        x = event.clientX;
        mouseMove() ;
        return false ;
    }

    if (n4||n6)
    {
        window.captureEvents(Event.MOUSEMOVE);
        if (n4 ) {
          window.onMouseDown   = mouse_nn ;
          window.onMouseUp     = mouse_nn ;
          window.onMouseMove   = mouse_nn ;
        } else {
          document.onmousedown = mouse_nn ;
          document.onmouseup   = mouse_nn ;
          document.onmousemove = mouse_nn ;
        }
    }

    if (ie||o6){
        document.onmousemove   = mouse_ie ;
    }

    //  drag'n'drop stuff
    var gz      =  100  ; // global z-index
    var gp      =  "x1" ; // current picture
    var bDown   = false ; // state of the mouse
    var X       = 0     ; // x at start of move
    var Y       = 0     ; // y at start of move
    var L       = 0     ; // Start Left
    var T       = 0     ; // Start Top
    var P               ; // which picture is moving
    var timeout = 0     ; // timer object
    var fast    = 2000  ; // sleep (milliseconds)
    var nThis   = 0     ; // current timed picture

    var oWidth  = 0     ; // old Width
    var oHeight = 0     ; // old Height
    var oPhoto  = 0     ; // old Photo URL

    var bOnce   = true  ; // debugging flag

    var bTitles = true  ; // enable/disable titles

    function seT(p,P)
    {
        var result ;
        try {
       // alert("document.getElementById('" + p +"')" + P)
          result = eval("document.getElementById('" + p +"')" + P) ;
        } catch ( e ) {} ;
        return result ;
    }

    function set(p,P)
    {
        var result ;
        try {
          result = eval("document." + p + P) ;
        } catch ( e ) {
          result = seT(p,P) ;
        } ;
        return result ;
    }

    function highlight(p)
    {
        set(gp , ".style.border      ='solid wheat 8px;'" ) ;
        set(gp , ".style.borderTop   ='solid wheat 38px;'") ;
        if ( oWidth && oHeight ) {
          set(gp,".src   ='" + fixup(oPhoto) + "'" ) ;
        }
        gp = p  ;

        if ( timeout ) {
          oWidth  = set(p,".width"  ) ;
          oHeight = set(p,".height" ) ;
          oPhoto  = set(p,".src"    ) ;
          var pp  = oPhoto.replace("Thumbs/","Images/") ;
          set(p,".src='" + fixup(pp) + "'") ;

          var cx  = parseInt(set(p,".style.left")) ;
          var cy  = parseInt(set(p,".style.top"))  ;

          window.scroll(cx,cy) ;

        } else {
          oWidth  = 0 ;
          oHeight = 0 ;
        }

        set( p , ".style.zIndex=" + (++gz)                ) ;
        set( p , ".style.border='solid white 8px;'"       ) ;
        set( p , ".style.borderTop   ='solid white 38px;'") ;
      //set( p , ".style.filter.alpha.opacity=50") ;
      //if ( timeout ) {
      //  set( p , ".style.MozOpacity=0.50;") ;
      //}
        if ( bTitles ) {
          var s = "t" + p.substr(1) ;
          set(s , ".style.zIndex=" + (++gz)  ) ;
        }
    }

    function mouseOver(p)
    {
        if ( !bDown && !timeout ) {
          highlight(p) ;
          nThis = p.substr(1) ;
        }
    }

    function mouseDown(p)
    {
        bDown  = true ;
        X      = x    ;
        Y      = y    ;
        P      = p    ;
        L      = parseInt(set(p,".style.left")) ;
        T      = parseInt(set(p,".style.top ")) ;
    }

    function click(p)
    {
        if ( timeout ) Stop() ;
        else {
          var oPhoto  = set(p,".src"    ) ;
          var pp ;
          if ( oPhoto.indexOf("Thumbs/") != -1 ) {
            pp  = oPhoto.replace("Thumbs/","Images/") ;
          } else {
            pp  = oPhoto.replace("Images/","Thumbs/") ;
          }

          set(p,".src='" + fixup(pp) + "'") ;
          set(p, ".style.zIndex=" + (++gz)  ) ;

          var s = "t" + p.substr(1) ;
          set(s , ".style.zIndex=" + (++gz)  ) ;
        }
    }

    function mouseUp(p)
    {
        if ( bDown ) {
          var dx = abs(x-X)    ;
          var dy = abs(y-Y)    ;
          if ( (dx+dy) < 4 ) click(p) ;
        }
        bDown = false ;
    }

    function mouseMove()
    {
        if ( bDown ) {
          var l = L+x-X    ;
          var t = T+y-Y    ;
          set(P, ".style.left = " + l) ;
          set(P, ".style.top  = " + t) ;
          if ( bTitles ) {
            var s = "t" + P.substr(1)    ;
            set(s, ".style.left = " + (l+10)) ;
            set(s, ".style.top  = " + (t+10)) ;
          }
        }
    }

    function theTitle(i)
    {
        var r = unescape(photos[i]) ;
        var e;
        try {
        	var R = unescape(captions[i]);
        	if ( R != 'None' ) r=R;
        } catch(e) {} ;
        if ( r.lastIndexOf("/") != -1 ) {
          r = r.substr(r.lastIndexOf("/")+1) ;
        }
        if ( r.lastIndexOf(".") != -1 ) {
          r = r.substr(0,r.lastIndexOf(".")) ;
        }
        while ( r.indexOf(' ') != -1 ) {
          r = r.replace(" ","&nbsp;") ;
        }
        return r ;
    }

    function println(s) { document.write(s+"\n") ; }
    function Println(s) { println(s) ; if ( bOnce ) alert(s) ; bOnce = false ; }

    function theDoc(nPhotos,nColPixels,nRowPixels,nColRandom,nRowRandom)
    {
        println("<table>") ;
        var bTrailer = false ;
        var nRow     = 0     ;
        var nCol     = 0     ;
        for ( var i = 0 ; i < photos.length ; i++ ) {
          if ( (i % nPhotos) == 0 ) {
             println("<tr>") ;
             bTrailer = true ;
          }

          var n = "'x" + i + "'" ;
          println( "<td><img name=" + n
                 + " galleryimg = 'no' src=\"" + photos[i] + "\""
                 + " onMouseover=\"mouseOver(" + n +");\""
                 + " onMousedown=\"mouseDown(" + n +");\""
                 + " onMouseup=\"mouseUp("     + n +");\""
                 + " style=\"border:solid wheat 8px;border-top:solid wheat 38px;left:0px;top:0px;"
                 + " position:absolute;z-index:0;\"></td>"
                 ) ;


          nCol ++ ;
          if ( ((i+1)%nPhotos)== 0 ) {
            nRow ++          ;
            nCol = 0         ;
            println("</tr>") ;
            bTrailer = false ;
          }
        }
        if ( bTrailer ) println("</tr>") ;
        println("</table>") ;

        for ( var i = 0 ; i < photos.length ; i++ ) {
          if ( bTitles ) {
            var n = '"t' + i + '"' ;
            println( "<p id=" + n
                 + " style=\"left:100px;top:100px;color:red;"
                 + " position:absolute;z-index:1000;\">"
                 + theTitle(i)
                 + " </p>"
                 ) ;
          }
        }

        Arrange(nPhotos,nColPixels,nRowPixels,nColRandom,nRowRandom);
    }

    function Arrange(nPhotos,nColPixels,nRowPixels,nColRandom,nRowRandom)
    {
        var nRow     = 0 ;
        var nCol     = 0 ;
        var bRandom  = nColPixels == 0 || nRowPixels == 0 ;

        for ( i = 0 ; i < photos.length ; i++ )
        {
          var l = nCol * nColPixels + Math.round(Math.random()* nColRandom) ;
          var t = nRow * nRowPixels + Math.round(Math.random()* nRowRandom) ;
          var n = "x" + i  ;
          set(n,".style.top="    + t    ) ;
          set(n,".style.left="   + l    ) ;
          set(n,".style.zIndex=" + (gz++) ) ;

          if ( bTitles ) {
            n = "t" + i ;
            set(n,".style.left="   + (l+10) ) ;
            set(n,".style.top="    + (t+10) ) ;
            set(n,".style.zIndex=" + (gz++) ) ;
          }

          nCol ++ ;
          if ( ((i+1)%nPhotos ) == 0 ) {
            nRow ++          ;
            nCol = 0         ;
          }
        }
    }

    function nextImage()
    {
        var bStopping = false ;

        if ( nThis >= photos.length ) {
          bStopping = true ;
          nThis = 0 ;
        }

        highlight("x" + nThis) ;

        if ( bStopping )   Stop()   ;
        else               nThis ++ ;
    }

    function Stop()
    {
        if ( timeout ) {
          window.clearTimeout(timeout) ;
          timeout  = 0  ;
          Small()       ;
        }
        //window.focus(1) ;
    }

	function Stop2(newLocation)
	{
	//	alert("Stop2 " + newLocation) ;
		try {
			window.stop() ; // stop loading (needed for Safari)
		} catch ( e ) {} ;
		location = newLocation ;
	}

    function changePhotos(first,second)
    {
        if ( !timeout ) {
          for ( var i = 0 ; i < photos.length ; i++ ) {
            var  x       = "x" + i ;
            var  oPhoto  = set(x,".src"    ) ;
            if ( oPhoto.indexOf(first) >= 0 ) {
              var pp  = oPhoto.replace(first,second) ;
              set(x,".src='" + fixup(pp) + "'") ;
            }
          }
        }
    }

    function Small() { changePhotos("Images/","Thumbs/") ;  }
    function Large() { changePhotos("Thumbs/","Images/") ;  }

    function Run()
    {
        Small() ;
        if (  nThis > photos.length ) nThis = 0 ;
        if ( !timeout ) {
          timeout = window.setInterval("nextImage()",fast) ;
        }
        if ( nThis == 1 ) nThis -- ;
        nextImage() ;
        //window.focus(2) ;
    }
</script>

<h1>$title$</h1>
<input type="button"  name="run"     value="Run"     tabindex="1"   onClick="Run()"/>
<input type="button"  name="stop"    value="Stop"    tabindex="2"   onClick="Stop()"/>
<input type="button"  name="grid"    value="Grid"    tabindex="3"   onClick="Arrange(3,280,290, 0, 0)"/>
<input type="button"  name="fridge"  value="Fridge"  tabindex="4"   onClick="Arrange(5,180,200,30,30)"/>
<input type="button"  name="wide"    value="Wide"    tabindex="5"   onClick="Arrange(8,150,180,40,40)"/>
<input type="button"  name="random"  value="Random"  tabindex="6"   onClick="Arrange(1,0,0,800,500)"/>
<input type="button"  name="small"   value="Small"   tabindex="8"   onClick="Small()"/>
<input type="button"  name="large"   value="Large"   tabindex="9"   onClick="Large()"/>
<div style="left:200px;top:80px;position:absolute;">

<script language="JavaScript" TYPE="text/javascript">
<!--

photos = [ $photos:{photo | "Thumbs/$photo.file$",
}$ '']; photos.pop();


captions = [ $photos:{photo | "$photo.name$",
}$ '']; captions.pop();

for ( i = 0 ; i < captions.length ; i++ ) 
  if ( captions[i] == '&amp;nbsp;' )
    captions[i] = '&nbsp;'

theDoc(5,180,200,30,30) ;
var   images = new Array() ;
for ( i = 0 ; i < photos.length ; i++ ) {
  var p = photos[i] ;
  p = p.replace("Thumbs/","Images/") ;
  images[i] = new Image() ;
  images[i].src = p ;
  window.status = "loading " + p ;
}
window.status = "done loading images" ;
//-->
</script>
<hr>
<center>
  <p align="middle">
    <img src="/robinali.gif" width="120" align="right"><br>
    <a href="/">Home</a> <a>.........</a> <a href="/about.html">About</a>
  <br><br>
  Page design &copy; 1996-2013 Robin Mills / <a href="mailto:webmaster@clanmills.com">webmaster@clanmills.com</a><br><br>
Page Created: 2014-05-25 10:54:35
  </p>
</center>

</div>
</body>
</html>

