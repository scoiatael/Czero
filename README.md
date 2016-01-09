# Template for projects based on Maphoon

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
