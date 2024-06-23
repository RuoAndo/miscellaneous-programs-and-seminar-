twttext_input = LOAD 'logcat' USING TextLoader();
twttext_token = FOREACH twttext_input GENERATE FLATTEN(TOKENIZE($0));
twttext_group = GROUP twttext_token BY $0;
twttext_count = FOREACH twttext_group GENERATE $0, COUNT($1);
twttext_count_filter = FILTER twttext_count BY $1 >= 5;
twttext_result = ORDER twttext_count_filter BY $1 DESC;

dump twttext_result;