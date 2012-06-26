#!/bin/bash


function myFun
{
    local  __resultvar="${@: -1}"
    echo ----------------------------
    echo "__resultvar = $__resultvar"
    echo ----------------------------
    local  myresult=$($1)
    eval $__resultvar="'$myresult'"
}


myFun "ls -alt" return_value
echo "return_value = $return_value"

# That's all Folks!
##
