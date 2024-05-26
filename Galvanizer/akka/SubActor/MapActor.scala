import akka.actor._
import scala.collection.mutable.ListBuffer

/*
case class MapCompleted()
case class ReduceCompleted()
*/

class MapActor extends Actor {

  import context._

  val mapOutputs = new ListBuffer[MapOutput]
  val reduceInputs = new ListBuffer[ReduceInput]
  val reduceOutputs = new ListBuffer[ReduceOutput]

  // メッセージハンドラとなるメソッド
  def receive = {
    case s: String =>
    	    println("mapActor: %s".format(s))
	    // println(s.mkString(","))
	    
	    val words = new ListBuffer[(String, Int)]
  	    s.toString.split(' ').foreach(words += _->1)
            println("List :" + words)
	    // sender ! MapOutput(words.toList)
            // stop(self)
	    
	    
    case _ => println("unknown")
  }
}

