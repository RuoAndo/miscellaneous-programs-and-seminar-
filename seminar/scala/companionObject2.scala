class URL ( val hostname:String, val portNo:Int, val path:Seq[String] ) {

      if( hostname == null ) throw new IllegalArgumentException( "hostname is null" )
      private def this( hostname:String, path:Seq[String] ) = this(hostname, 443, path)

      def toURL = new java.net.URL( this.toString )

      override def toString = {
      	       val portString = if( portNo == 443 ) "" else ":" + portNo
      	       val pathString = if( path.isEmpty ) "" else path.mkString("/")
      	       "https://%s%s/%s" format( hostname, portString, pathString )
      }
}

object URL {

  def apply( hostname:String, portNo:Int, path:Seq[String] ) = new URL( hostname, portNo, path )
  def apply( hostname:String, path:Seq[String] ) = new URL( hostname, 443, path )

  def fromURL( url:java.net.URL) = {
      new HttpURL ( url.getHost,
      	  if ( url.getPort == -1 ) 443 else url.getPort ,
	  url.getPath.split("/").dropWhile( _.isEmpty)
	  )
  }
}

object Main {
  def main(args: Array[String]) {
    val url = URL("https://www.google.com", List("node","443"))
    println( url )
  }
}
