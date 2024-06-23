class HttpURL ( host:String, port:Int, path:Seq[String] ) {

  if( host == null ) throw new IllegalArgumentException( "hostname is null" )

  /* in the case that port number is missing */
  def this( host:String, path:Seq[String] ) = this( host, 443, path )

  def toURL = new java.net.URL( this.toString )

  override def toString = {
    val portString = if( port == 443 ) "" else ":" + port
    val pathString = if( path.isEmpty ) "" else path.mkString("/")
    "https://%s%s/%s" format( host, portString, pathString )
  }
}

object LocalHostHttpURL {

       println("LocalHostHttpURL is created")

       val defaults = new HttpURL( "localhost", Seq.empty[String] )
       def port ( port:Int ) = new HttpURL( "localhost", port, Seq.empty[String])
}

/*
object URLs {
  def main(args: Array[String]) {
    LocalHostHttpURL.defaults
    LocalHostHttpURL.port(443)
    println(HttpURL)
  }
}
*/

