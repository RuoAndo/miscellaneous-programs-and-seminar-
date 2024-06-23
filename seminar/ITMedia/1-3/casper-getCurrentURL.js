// casperjs 6.js --url="https://t.co/c2G1XDExVW"

var casper = require('casper').create({
	pageSettings: {
	    webSecurityEnabled: false
	}
    });

var url = casper.cli.get('url'); 

casper.userAgent('Mozilla/5.0 (Macintosh; Intel Mac OS X 10_8_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/27.0.1453.116 Safari/537.36 ');
casper.start(url,function(){

    });
casper.then(function(){
	this.wait(3000, function() {
		// this.echo("ロード中");
	    });
    });

casper.then(function () {
	this.echo( this.getCurrentUrl());
	casper.capture("nodejs.png");
    });

casper.run();
 