twt_input = LOAD 'logcat' USING TextLoader();
twt_token = FOREACH twt_input GENERATE FLATTEN(TOKENIZE($0));
twt_group = GROUP twt_token BY $0;
dump twt_group

