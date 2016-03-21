
BISON_PATCH_PIPELINE=\
	sed -e "s/yystpcpy (yyres, yystr) - yyres/(YYSIZE_T) (yystpcpy (yyres, yystr) - yyres)/" \
	  -e "s/YYSIZE_T yysize = yyssp - yyss + 1/YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss) + 1/" \
# intentionally empty
