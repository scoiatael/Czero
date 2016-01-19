# This is going to be a simple imperative language compiler

Based on maphoon parser and translating to llvm IR.
More updates this file are about to come soon.

All commands assume CWD is repository root.

### build
```
cd build && cmake .. && make
```

### clean
```
cd build && make clean
```


### remove cmake artifacts
 (restores repo to pristine state)
```
cd build && make clean && make extra-clean
```
