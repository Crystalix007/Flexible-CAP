# Demonstration of templates being re-written for a specific argument specification

## Requirements

* mustache
* bison (to compile)
* flex (to compile)

## How to re-write

```sh
mustache demo.yml ../templates/<file>
```

To save the result to a file:

```sh
mustache demo.yml ../templates/<file> /path/to/output/file
```

## How to run

Run `mustache` on all the templates in `../templates/`, and on _`main.cpp`_ (in this folder) to an empty output directory (`/path/to/output`).
Then:

```sh
bison /path/to/output/*.yy
flex /path/to/output/*.ll
g++ /path/to/output/*.cpp /path/to/output*.cc -o demo
/path/to/output/demo
```
