// ------------------------------------
// bmi - body mass index
// ------------------------------------
function bmi(lbs, ins)
{
   h2    = ins * ins;
   bmi   = lbs/h2 * 703
   f_bmi = Math.floor(bmi);
   diff  = bmi - f_bmi;
   diff  = diff * 10;
   diff  = Math.round(diff);
   if ( diff == 10 ) {
      // Need to bump up the whole thing instead
      f_bmi += 1 ;
      diff   = 0 ;
   }
   bmi = f_bmi + "." + diff ;
   return bmi;
}

// ------------------------------------
// Exit - terminate program
// ------------------------------------
function Exit()
{
  WScript.Quit(1) ;
}

// ------------------------------------
// Print - print a string
// ------------------------------------
function Print(s)
{
    WScript.Echo(s) ;
}

// ====================================
// Main program
// ====================================
Args   = WScript.Arguments;
if ( Args.length < 2 || Args.length > 2 ) {
  Print("Syntax: bmi.js weight-pounds height-inches")  ;
  Exit() ;
}

w = parseInt(Args(0));
h = parseInt(Args(1));

Print("BMI = " + bmi(w,h)) ;