#pragma once

/*
* Earlier the below exports were only for X86, but with invent of /clr:pure,
* they need to be defined for all the platform.
*/

/*
* Functions to access user-visible, per-process variables
*/

/*
* Macro to construct the name of the access function from the variable
* name.
*/
#define AFNAME(var) __p_ ## var

/*
* Macro to construct the access function's return value from the variable
* name.
*/
#define AFRET(var)  &var

/*
***
***  Template
***

_CRTIMP __cdecl
AFNAME() (void)
{
return AFRET();
}

***
***
***
*/