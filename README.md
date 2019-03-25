# Mojave 
**A hand made inverted index engine.**

---

## Build
+ CMAKE VERSION **3.13** is **REQUIRED**
+ Unix/Liux is required, Windows is not supported 
``` bash
$ cmake .
$ make
```
## Usage
Just use command line and move your fingers.
+ **Create a index** 
``` bash
$ ./mojave create [-I filename...] [-S stopword-file] [-O outputfile](optional) [-Q1](optional)
```
+ **Search a string from a index** 
``` bash
$ ./mojave find [-I filename] [-s string]
```
+ **Consistent query** 
``` bash
$ ./mojave watch [-I filename] [-F stanard-query-file](optional)
```

**Standard Search File Example**:
```
apple
world
yuki
katana
jinja
hanas
\q
```
---



