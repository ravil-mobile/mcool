## COOL -  Classroom Object Oriented Language

This is a C++17/LLVM implementation of the language.

#### Dependencies

1) `gcc` with C++17 support (e.g., v11.3)
2) `flex v2.6`
3) `bison v3.5`
4) `llvm v12.0.1`
5) `googletest v1.10`

#### Installation
You can install `cool` on your machine as follows
```bash
mkdir build && cd build
cmake ../mcool -DCMAKE_INSTALL_PREFIX=<installation directory>
make -j2
make install
```

If you experience problems with installing dependencies you
can build a Docker image
```bash
docker build -t mcool .
```

#### Example

```bash
$ cat ./fibonacci.cl
```

```java
class Main inherits IO {
  main(): Object {{
    out_string("Enter an integer greater-than or equal-to 0: ");

    let input: Int <- in_int() in
      if input < 0 then
        out_string("ERROR: Number must be greater-than or equal-to 0\n")
      else {
        out_string("The fibonacci number of ").out_int(input);
        out_string(" is ").out_int(fibonacci(input));
        out_string("\n");
      }
      fi;
  }};

  fibonacci(num: Int): Int {
    if num = 0 then 0 else { if num = 1 then 1 else fibonacci(num - 1) + fibonacci(num - 2) fi; } fi
  };
};
```

```bash
$ mcool -i ./fibonacci.cl -o ./fibonacci
$ clang ./fibonacci.o -o ./fibonacci
$ ./fibonacci
```

Note, we use `clang` (or `gcc`) as a linker.

#### Miscellaneous

Use `mcool --help` to see all available compiler options

#### Current Status

|      Systems      |       Status       |        Tests       |
|:-----------------:|:------------------:|:------------------:|
|       Lexer       | :heavy_check_mark: | :heavy_check_mark: |
|       Parser      | :heavy_check_mark: | :heavy_check_mark: |
|   Type Checking   | :heavy_check_mark: | :heavy_check_mark: |
|  Code Generation  | :heavy_check_mark: |         :x:        |
| Garbage Collector |         :x:        |         :x:        |