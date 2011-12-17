var indent = 0 ;
function debug(o)
{
  this.counter = function(o)
  {
    var   c = 0 ;
    for ( var x in o ) c++ ;
    return c ;
  }

  this.Indent = function()
  {
    print() ;
    var       b = ""   ;
    for ( var y = 0 ; y < indent ; y++ )  b += "  " ;
    $.write(b) ;
  }

  if ( typeof(o) == "string" ) {
    if ( o.length > 200 ) {
      var L = o.length ;
      o = o.substr(0,100) + " ... deleted length = " + L + " ..." ;
    }
    $.write('"' + o + '"') ;
  
  } else if ( typeof(o) == "boolean" ) {
    $.write(o) ;

  } else if ( typeof(o) == "function" )  {
    var s = o.toString() ;
    var x = "" ;

    // remove all '\n' 
    // cant get replace to work!
    for ( var i = 0 ; i < s.length ; i++ ) {
      if ( s[i] != '\n' ) {
        x += s[i] ;
      }
    }
    
    o = x ;
    if ( o.length > 200 ) {
      var L = o.length ;
      o = o.substr(0,100) + " ... deleted length = " + L + " ..." ;
    }

    $.write(o) ;
  
  } else if ( typeof(o) == "number" ) {
    $.write(o) ;

  } else if ( o instanceof Array ) {
    var c = 0 ;
    $.write( "[ " ) ;
    for ( var i = 0 ; i < o.length ; i++ ) {
      var bLast = (i+1) == o.length ;
      debug(o[i]) ;
      if ( !bLast ) $.write(", ") ;
    }
    $.write( "]" ) ;
  
  } else if ( o instanceof Date ) {
    $.write("-> " + o + " <-") ;
 
  } else if ( o instanceof Object ) {
    var c = 0 ;
    $.write( "{ " ) ;
    indent ++    ;
    var vc = counter(o) ;
    var v  = 0 ;
    for ( var i in o ) {
      var e ;
      var bLast = (1+v++) == vc ;
      Indent()     ;
      $.write(i + " : ") ;

      try {
        debug(o[i]) ;
      } catch (e) {
        $.write("- oops -") ;
      }

      if ( !bLast ) $.write(" ,") ;
    }
    indent-- ;
    Indent() ;
    $.write( "}" ) ;
  
  } else {
    // probably shouldn't come here (booleans etc? - don't know!)
    $.write(s) ;
  }
  if ( !indent ) print() ;
}
