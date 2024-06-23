val t1 = Tuple1(1)
val t2 = (1, 2)
val t3 = (1, 2, 3)

printTuple(t1)
printTuple(t2)
printTuple(t3)

def printTuple(t: Product) {
  t match {
    case Tuple1(a) => println("length = 1, content = " + a)
    case (a, b) => println("length = 2, content = " + a + "," + b)
    case (a, b, c) => println("length = 3, content = " + a + "," + b + "," + c)
  }
}
