## COOL -  Classroom Object Oriented Language

This a C++17/LLVM implementation of the language.

#### Dependencies

1) `gcc` with C++17 support (e.g., v11.3)
2) `flex v2.6`
3) `bison v3.5`
4) `llvm v12.0.1`
5) `googletest v1.10`

#### Installation
You can install `cool` on your machine as follows
```
mkdir build && cd build
cmake ../mcool -DCMAKE_INSTALL_PREFIX=<installation directory>
make -j2
make install
```

If you experience problems with installing dependencies you
can build a Docker image
```
docker build -t mcool .
```

#### Example

```
$ cat ./factorial.cl

class Main inherits IO {
  main(): Object {{
    out_string("Enter an integer greater-than or equal-to 0: ");

    let input: Int <- in_int() in
      if input < 0 then
        out_string("ERROR: Number must be greater-than or equal-to 0\n")
      else {
        out_string("The factorial of ").out_int(input);
        out_string(" is ").out_int(factorial(input));
        out_string("\n");
      }
      fi;
  }};

  factorial(num: Int): Int {
    if num = 0 then 1 else num * factorial(num - 1) fi
  };
};

$ mcool -i ./factorial.cl -o ./factorial.ll
$ llc ./factorial.ll
$ clang ./factorial.s -o ./factorial
$ ./factorial
```