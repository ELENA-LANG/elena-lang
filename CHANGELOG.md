## upcoming ELENA 6.0
*12.06.2022*

- ELENA
  - binary incompatible due to module structure change
- ELC
  - a warning shown when an object cannot be converted into another one
  - a warning shown when an object does not handle a message (for strong-typed ones)
  - #511 : Infinite extension **populate** call
  - #514 : Incorrect warning message for unhandled function message 
  - #516 : Dispatch handle : dispatching to an expression 
- TOOLS
   - #323 : Run ELENA code without build

## ELENA 5.0
*14.07.2020*

- ELENA
  - binary incompatible due to major version change
  - Explicit namespaces
  - Flat library hierarchy
  - nested code brackets
  - #500 : Nested code blocks
  - #492 : Yield statement 
  - #380 : Run-time extension dispatching 
  - #494 : Method result multi-dispatching 
  - #495 : Inheritable static methods 
  - #501 : Modificator **new** can be used with a variable
- ELC
  - deep code refactoring - treamlining code generation, increasing the compilation speed
  - #489 : Creating a variadic argument enumerator
  - #490 : Passing a variadic argument list in sub call
  - #491 : The message target is corrupt when a variadic argument is used in sub call 
  - #381 : const static property
  - #162 : compiler redux - embedded constructor call 
  - #505 : constructor with variadic argument list 
  - #498 : Compile-time dispatching nil argument 
  - #472 : Embeddable property is not called correctly
