#!/usr/bin/env python                                                   
# -*- coding:utf-8 -*-                                                  
from requests_oauthlib import OAuth1Session
import json
import sys 
import random
import math

argvs = sys.argv  
argc = len(argvs) 
#print argvs
#print argc

consumer_key = argvs[2]
consumer_secret = argvs[3]
access_token = argvs[4]
access_token_secret = argvs[5]

### Constants                                                           
oath_key_dict = {
    "consumer_key": consumer_key,
    "consumer_secret": consumer_secret,
    "access_token": access_token,
    "access_token_secret": access_token_secret
}

### Functions                                                           
def main():

    f = open( "follow", "a" )
    search_word = argvs[1]
    #print "py:" + search_word
    tweets = tweet_search(search_word.decode('utf-8'), oath_key_dict)
    for tweet in tweets["statuses"]:
        tweet_id = tweet[u'id_str']
        text = tweet[u'text']
        created_at = tweet[u'created_at']
        user_id = tweet[u'user'][u'id_str']
        user_description = tweet[u'user'][u'description']
        screen_name = tweet[u'user'][u'screen_name']
        user_name = tweet[u'user'][u'name']

        print text.encode('utf_8')

        f.write(screen_name)
        f.write("\n") 

    f.close
    return

    

def create_oath_session(oath_key_dict):
    oath = OAuth1Session(
    oath_key_dict["consumer_key"],
    oath_key_dict["consumer_secret"],
    oath_key_dict["access_token"],
    oath_key_dict["access_token_secret"]
    )
    return oath

def tweet_search(search_word, oath_key_dict):
    url = "https://api.twitter.com/1.1/search/tweets.json?"
    params = {
        "q": unicode(search_word),
        "lang": "en",
        "result_type": "recent",
        "count": "10"
        }
    oath = create_oath_session(oath_key_dict)
    responce = oath.get(url, params = params)
    if responce.status_code != 200:
        return None
    tweets = json.loads(responce.text)
    return tweets

### Execute                                                             
if __name__ == "__main__":
    main()

