class A {
  x : Int <- 3 + 10;
  str : String <- "Hello world";
  variable : Bool <- true;
  obj : A;
  main(x: Int, y : ClassD) : Object {
    {
      10 / 5;
      x <- 123 + 777 * x;
      variable;

      obj <- new B;
      --not (10 < 2 + 5);

      let i : Int <- 0 in {
        while i <= 10 loop
          i <- i + 1
        pool;
      };

      isvoid x;
      ~ (1 + 10);
      str.length();
    }
  };
  foo(someVar : String) : Object {
    {
      case someVar of
        a: Int => 1;
        b: String => "hello";
      esac;
    }
  };
};

class B inherits A {
  bar(x : Int, z : Int) : Int {
    {
     while true loop
        x <- x + 1
      pool;

      if 1 < 2 then true else false fi;

      let x : Int in { x - 8; };
      not  x < z;
      new A;
      x;
    }
  };
};
