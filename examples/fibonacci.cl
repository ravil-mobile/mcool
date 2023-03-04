
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

