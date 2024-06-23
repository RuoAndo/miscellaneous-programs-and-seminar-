val tpl1 = Tuple1((1))
val tpl2 = (1, 2)
val tpl3 = (1, 2, 3)

displayTuple(tpl1)
displayTuple(tpl2)
displayTuple(tpl3)

def displayTuple(t: Product) {
  t match {
    case (a, b, c) => displayln("len = 3, value = " + a + "," + b + "," + c)
    case (a, b) => displayln("len = 2, value = " + a + "," + b)
    case (a) => displayln("len = 1, value = " + a)
  }
}
