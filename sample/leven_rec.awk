function leven_rec(s1, s2,  ct, c1, c2, t1, t2, m1, m2) {
    if ( s2 == "" ) { return length(s1); }
    if ( s1 == "" ) { return length(s2); }
    c1 = substr(s1, 1, 1);    # 先頭の文字
    c2 = substr(s2, 1 ,1);    # 先頭の文字
    t1 = substr(s1, 2);       # 残余の文字列
    t2 = substr(s2, 2);       # 残余の文字列
    ct = leven_rec(t1, t2);
    if ( c1 == c2 ) { return ct; }
    m1 = leven_rec(t1, s2);
    m2 = leven_rec(s1, t2);
    if ( ct > m1 ) { ct = m1; }
    if ( ct > m2 ) { ct = m2; }
    return ct+1;
}
