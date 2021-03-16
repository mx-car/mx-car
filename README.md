# car
## subrepositiories
In order to clone the subrepositiories run:
```
mr -c mrconfig update
```
## Compilation
### issues
#### M4l_math   
* Compilation might fail due to a missing math library since we are using a newer compiler than the stardart issue. In this case simply copy the library from an older gccarmnoneaebi:  

  ```
  cp ~/.platformio/packages/toolchain-gccarmnoneeabi@1.50401.190816/arm-none-eabi/lib/libarm_cortexM4l_math.a ~/.platformio/packages/toolchain-gccarmnoneeabi/arm-none-eabi/lib
  ```

  For manual copying you can look for the missing library under ``~/.platformio/packages``.  Try `find ~/.platformio -iname libarm_cortexM4l_math.a` to locate the missing library if the command above doesn't work directly.
