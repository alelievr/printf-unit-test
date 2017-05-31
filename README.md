# printf-unit-test

## Install

clone or download this repo and place it in the parent folder of your ft_printf dir.

you can configure the path of your printf with the var PRINTFDIR in the Makefile

## Run

`make f` or `./run_test`

## Usage
./run_test \<options> \<converters>  
  -e: stop to the first error / segfault  
  -q: disable errer/segv/timeout output  
  -r: disable speed test  
  -d: debug mode  
  -h: display help  
  -f \<fname>: output in the specified file  

## Test System

this is a brute-force based test for printf, it will check all compilable (with -Wformat flag) printf converters and flags.  
supported converters: idDoOuUxXcCsSpaAeEfFgG  
supported modifiers: ".#0-+ "  

current number of tested formats: 3.4M (without float converters)

