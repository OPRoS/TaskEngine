/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "interpreter/RFSMException.h"

// ���ڿ� buf���� ���ڿ� a�� ��� ����
int delstring(char *buf, char *a)
{
    int n=0, k=0, s=0, i=0;
    for(i=0; buf[i]!='\0'; i++){
       if(buf[i]==a[n]){ 
           k = i;
           while(buf[i]==a[n])  {
               if(a[++n]=='\0'){           
                   for(k; buf[k+n]!='\0'; k++){        
                       s=1;
                       buf[k] = buf[k+n];
                   }
                   buf[k]='\0';
                   break;
               }
               i++;
           } 
           n=0;
           i--;
       }
    } 
    return s;
}

void produceError(pANTLR3_BASE_RECOGNIZER _pRec, pANTLR3_UINT8* _tokenNames) 
{
	printf("*******************");
	
	/*
	if(_pRec->exception != NULL){
	 	char buffer[100];
        sprintf(buffer, "\%s", _pRec->exception->name);
        printf("---------------EEEEEEEERRRRR\n\n");
        printf(buffer);
	}
	*/
	
}

void produceError4Lexer(pANTLR3_BASE_RECOGNIZER _pRec, pANTLR3_UINT8* _tokenNames) 
{
	
	pANTLR3_LEXER			lexer;
	pANTLR3_EXCEPTION	    ex;
	pANTLR3_STRING			ftext;

    lexer   = (pANTLR3_LEXER)(_pRec->super);
	ex		= lexer->rec->state->exception;

	//printf("�̰��� �׽�Ʈ");//���� ����� ���� �Ʒ��� �ݵ�� ��ġ�ؾ��Ѵ�.c�� �׷���

	// See if there is a 'filename' we can use
    //
    if	(ex->name == NULL)
    {
		ANTLR3_FPRINTF(stderr, "-unknown source-(");
    }
    else
    {
		ftext = ex->streamName->to8(ex->streamName);
		ANTLR3_FPRINTF(stderr, "%s(", ftext->chars);
    }

    //ANTLR3_FPRINTF(stderr, "%d) ", _pRec->state->exception->line);
    //ANTLR3_FPRINTF(stderr, ": lexer error %d :\n\t%s at offset %d, ", ex->type, (pANTLR3_UINT8)(ex->message), ex->charPositionInLine+1);
	ANTLR3_FPRINTF(stderr, "%d) : error : Unknown token,", _pRec->state->exception->line);
	{
		ANTLR3_INT32	width;

		width	= ANTLR3_UINT32_CAST(( (pANTLR3_UINT8)(lexer->input->data) + (lexer->input->size(lexer->input) )) - (pANTLR3_UINT8)(ex->index));

		if	(width >= 1)
		{			
			if	(isprint(ex->c))
			{
				ANTLR3_FPRINTF(stderr, "near '%c' :\r\n", ex->c);
			}
			else
			{
				ANTLR3_FPRINTF(stderr, "near char(%#02X) :\r\n", (ANTLR3_UINT8)(ex->c));
			}
			ANTLR3_FPRINTF(stderr, "\t%.*s\r\n", width > 20 ? 20 : width ,((pANTLR3_UINT8)ex->index));
		}
		else
		{
			//ANTLR3_FPRINTF(stderr, "(end of input).\n\t This indicates a poorly specified lexer RULE\n\t or unterminated input element such as: \"STRING[\"]\n");
			//ANTLR3_FPRINTF(stderr, "\t The lexer was matching from line %d, offset %d, which\n\t ", 
			//					(ANTLR3_UINT32)(lexer->rec->state->tokenStartLine),
			//					(ANTLR3_UINT32)(lexer->rec->state->tokenStartCharPositionInLine)
			//					);
			ANTLR3_FPRINTF(stderr, "\t Line %d, offset %d, which ", 
								(ANTLR3_UINT32)(lexer->rec->state->tokenStartLine),
								(ANTLR3_UINT32)(lexer->rec->state->tokenStartCharPositionInLine)
								);
			width = ANTLR3_UINT32_CAST(((pANTLR3_UINT8)(lexer->input->data)+(lexer->input->size(lexer->input))) - (pANTLR3_UINT8)(lexer->rec->state->tokenStartCharIndex));

			if	(width >= 1)
			{
				ANTLR3_FPRINTF(stderr, "looks like this:\r\n\t\t%.*s\r\n", width > 20 ? 20 : width ,(pANTLR3_UINT8)(lexer->rec->state->tokenStartCharIndex));
			}
			else
			{
				ANTLR3_FPRINTF(stderr, "is also the end of the line, so you must check your lexer rules\r\n");
			}
		}
	}

	_pRec->state->errorCount++;
}

/**
displayRecognitionError()�Լ��� ����
*/
void produceError4Parser(pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames)
{
	pANTLR3_PARSER			parser;
	pANTLR3_TREE_PARSER	    tparser;
	pANTLR3_INT_STREAM	    is;
	pANTLR3_STRING			ttext;
	pANTLR3_STRING			ftext;
	pANTLR3_EXCEPTION	    ex;
	pANTLR3_COMMON_TOKEN    theToken;
	pANTLR3_BASE_TREE	    theBaseTree;
	pANTLR3_COMMON_TREE	    theCommonTree;

	//char* p;
	FILE *out;
	out = fopen("ParErr.txt", "ab"); // ������ ������ ����
	ANTLR3_FPRINTF(out, "\r\n");

	// Retrieve some info for easy reading.
	//
	ex	    =		recognizer->state->exception;
	ttext   =		NULL;

	// See if there is a 'filename' we can use
	//
	if	(ex->streamName == NULL)
	{
		if	(((pANTLR3_COMMON_TOKEN)(ex->token))->type == ANTLR3_TOKEN_EOF)
		{
			ANTLR3_FPRINTF(out, "-end of input-(");
		}
		else
		{
			ANTLR3_FPRINTF(out, "-unknown source-(");
		}
	}
	else
	{   
		/* rwkim ����*/
		char curPath[200];
		getcwd(curPath, 200);
		ftext = ex->streamName->to8(ex->streamName);
		delstring(ftext->chars, curPath); //���� ��� ����
		ANTLR3_FPRINTF(out, "%s(", ftext->chars);
	}

	// Next comes the line number
	//

	ANTLR3_FPRINTF(out, "%d) ", recognizer->state->exception->line);
	//ANTLR3_FPRINTF(out, " : error %d : %s", 
	//									recognizer->state->exception->type,
	//				(pANTLR3_UINT8)	   (recognizer->state->exception->message));
	//������ ���� �ٲپ���
	ANTLR3_FPRINTF(out, " : %s", (pANTLR3_UINT8)(recognizer->state->exception->message));

	// How we determine the next piece is dependent on which thing raised the
	// error.
	//
	
	switch	(recognizer->type)
	{
	case	ANTLR3_TYPE_PARSER:

		// Prepare the knowledge we know we have
		//
		parser	    = (pANTLR3_PARSER) (recognizer->super);
		tparser	    = NULL;
		is			= parser->tstream->istream;
		theToken    = (pANTLR3_COMMON_TOKEN)(recognizer->state->exception->token);
		ttext	    = theToken->toString(theToken);

		/* rwkim ����*/
		//p = strchr(ttext->chars, '=');
		//ttext->chars = p;

		ANTLR3_FPRINTF(out, " at offset %d", recognizer->state->exception->charPositionInLine);
		if  (theToken != NULL)
		{
			if (theToken->type == ANTLR3_TOKEN_EOF)
			{
				ANTLR3_FPRINTF(out, ", at <EOF>");
			}
			else
			{
				// Guard against null text in a token
				//
				ANTLR3_FPRINTF(out, "\r\n    near %s\r\n    ", ttext == NULL ? (pANTLR3_UINT8)"<no text for the token>" : ttext->chars);
			}
		}
		break;

	case	ANTLR3_TYPE_TREE_PARSER:

		tparser		= (pANTLR3_TREE_PARSER) (recognizer->super);
		parser		= NULL;
		is			= tparser->ctnstream->tnstream->istream;
		theBaseTree	= (pANTLR3_BASE_TREE)(recognizer->state->exception->token);
		ttext		= theBaseTree->toStringTree(theBaseTree);

		if  (theBaseTree != NULL)
		{
			theCommonTree	= (pANTLR3_COMMON_TREE)	    theBaseTree->super;

			if	(theCommonTree != NULL)
			{
				theToken	= (pANTLR3_COMMON_TOKEN)    theBaseTree->getToken(theBaseTree);
			}
			ANTLR3_FPRINTF(out, ", at offset %d", theBaseTree->getCharPositionInLine(theBaseTree));
			ANTLR3_FPRINTF(out, ", near %s", ttext->chars);
		}
		break;

	default:

		ANTLR3_FPRINTF(out, "Base recognizer function displayRecognitionError called by unknown parser type - provide override for this function\r\n");
		return;
		break;
	}

	// Although this function should generally be provided by the implementation, this one
	// should be as helpful as possible for grammar developers and serve as an example
	// of what you can do with each exception type. In general, when you make up your
	// 'real' handler, you should debug the routine with all possible errors you expect
	// which will then let you be as specific as possible about all circumstances.
	//
	// Note that in the general case, errors thrown by tree parsers indicate a problem
	// with the output of the parser or with the tree grammar itself. The job of the parser
	// is to produce a perfect (in traversal terms) syntactically correct tree, so errors
	// at that stage should really be semantic errors that your own code determines and handles
	// in whatever way is appropriate.
	//
	switch  (ex->type)
	{
	case	ANTLR3_UNWANTED_TOKEN_EXCEPTION:

		// Indicates that the recognizer was fed a token which seesm to be
		// spurious input. We can detect this when the token that follows
		// this unwanted token would normally be part of the syntactically
		// correct stream. Then we can see that the token we are looking at
		// is just something that should not be there and throw this exception.
		//
		if	(tokenNames == NULL)
		{
			ANTLR3_FPRINTF(out, " : Extraneous input...");
		}
		else
		{
			if	(ex->expecting == ANTLR3_TOKEN_EOF)
			{
				ANTLR3_FPRINTF(out, " : Extraneous input - expected <EOF>\r\n");
			}
			else
			{
				ANTLR3_FPRINTF(out, " : Extraneous input - expected %s ...\r\n", tokenNames[ex->expecting]);
			}
		}
		break;

	case	ANTLR3_MISSING_TOKEN_EXCEPTION:

		// Indicates that the recognizer detected that the token we just
		// hit would be valid syntactically if preceeded by a particular 
		// token. Perhaps a missing ';' at line end or a missing ',' in an
		// expression list, and such like.
		//
		if	(tokenNames == NULL)
		{
			ANTLR3_FPRINTF(out, " : Missing token (%d)...\r\n", ex->expecting);
		}
		else
		{
			if	(ex->expecting == ANTLR3_TOKEN_EOF)
			{
				ANTLR3_FPRINTF(out, " : Missing <EOF>\r\n");
			}
			else
			{
				ANTLR3_FPRINTF(out, " : Missing %s \r\n", tokenNames[ex->expecting]);
			}
		}
		break;

	case	ANTLR3_RECOGNITION_EXCEPTION:

		// Indicates that the recognizer received a token
		// in the input that was not predicted. This is the basic exception type 
		// from which all others are derived. So we assume it was a syntax error.
		// You may get this if there are not more tokens and more are needed
		// to complete a parse for instance.
		//
		ANTLR3_FPRINTF(out, " : syntax error...\r\n");    
		break;

	case    ANTLR3_MISMATCHED_TOKEN_EXCEPTION:

		// We were expecting to see one thing and got another. This is the
		// most common error if we coudl not detect a missing or unwanted token.
		// Here you can spend your efforts to
		// derive more useful error messages based on the expected
		// token set and the last token and so on. The error following
		// bitmaps do a good job of reducing the set that we were looking
		// for down to something small. Knowing what you are parsing may be
		// able to allow you to be even more specific about an error.
		//
		if	(tokenNames == NULL)
		{
			ANTLR3_FPRINTF(out, " : syntax error...\r\n");
		}
		else
		{
			if	(ex->expecting == ANTLR3_TOKEN_EOF)
			{
				ANTLR3_FPRINTF(out, " : expected <EOF>\r\n");
			}
			else
			{
				ANTLR3_FPRINTF(out, " : expected %s ...\r\n", tokenNames[ex->expecting]);
			}
		}
		break;

	case	ANTLR3_NO_VIABLE_ALT_EXCEPTION:

		// We could not pick any alt decision from the input given
		// so god knows what happened - however when you examine your grammar,
		// you should. It means that at the point where the current token occurred
		// that the DFA indicates nowhere to go from here.
		//
		ANTLR3_FPRINTF(out, " : cannot match to any predicted input...\r\n");

		break;

	case	ANTLR3_MISMATCHED_SET_EXCEPTION:

		{
			ANTLR3_UINT32	  count;
			ANTLR3_UINT32	  bit;
			ANTLR3_UINT32	  size;
			ANTLR3_UINT32	  numbits;
			pANTLR3_BITSET	  errBits;

			// This means we were able to deal with one of a set of
			// possible tokens at this point, but we did not see any
			// member of that set.
			//
			ANTLR3_FPRINTF(out, " : unexpected input...\r\n  expected one of : ");

			// What tokens could we have accepted at this point in the
			// parse?
			//
			count   = 0;
			errBits = antlr3BitsetLoad		(ex->expectingSet);
			numbits = errBits->numBits		(errBits);
			size    = errBits->size			(errBits);

			if  (size > 0)
			{
				// However many tokens we could have dealt with here, it is usually
				// not useful to print ALL of the set here. I arbitrarily chose 8
				// here, but you should do whatever makes sense for you of course.
				// No token number 0, so look for bit 1 and on.
				//
				for	(bit = 1; bit < numbits && count < 8 && count < size; bit++)
				{
					// TODO: This doesn;t look right - should be asking if the bit is set!!
					//
					if  (tokenNames[bit])
					{
						ANTLR3_FPRINTF(out, "%s%s", count > 0 ? ", " : "", tokenNames[bit]); 
						count++;
					}
				}
				ANTLR3_FPRINTF(out, "\r\n");
			}
			else
			{
				ANTLR3_FPRINTF(out, "Actually dude, we didn't seem to be expecting anything here, or at least\r\n");
				ANTLR3_FPRINTF(out, "I could not work out what I was expecting, like so many of us these days!\r\n");
			}
		}
		break;

	case	ANTLR3_EARLY_EXIT_EXCEPTION:

		// We entered a loop requiring a number of token sequences
		// but found a token that ended that sequence earlier than
		// we should have done.
		//
		ANTLR3_FPRINTF(out, " : missing elements...\r\n");
		break;

	default:

		// We don't handle any other exceptions here, but you can
		// if you wish. If we get an exception that hits this point
		// then we are just going to report what we know about the
		// token.
		//
		ANTLR3_FPRINTF(out, " : syntax not recognized...\r\n");
		break;
	}

	// Here you have the token that was in error which if this is
	// the standard implementation will tell you the line and offset
	// and also record the address of the start of the line in the
	// input stream. You could therefore print the source line and so on.
	// Generally though, I would expect that your lexer/parser will keep
	// its own map of lines and source pointers or whatever as there
	// are a lot of specific things you need to know about the input
	// to do something like that.
	// Here is where you do it though :-).
	//

	fclose(out); 
}

/// Return how many syntax errors were detected by this recognizer
///

/*
void displayRecognitionError(pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames)
{
	pANTLR3_PARSER			parser;
	pANTLR3_TREE_PARSER	    tparser;
	pANTLR3_INT_STREAM	    is;
	pANTLR3_STRING			ttext;
	pANTLR3_STRING			ftext;
	pANTLR3_EXCEPTION	    ex;
	pANTLR3_COMMON_TOKEN    theToken;
	pANTLR3_BASE_TREE	    theBaseTree;
	pANTLR3_COMMON_TREE	    theCommonTree;

	// Retrieve some info for easy reading.
	//
	ex	    =		recognizer->state->exception;
	ttext   =		NULL;

	// See if there is a 'filename' we can use
	//
	if	(ex->streamName == NULL)
	{
		if	(((pANTLR3_COMMON_TOKEN)(ex->token))->type == ANTLR3_TOKEN_EOF)
		{
			ANTLR3_FPRINTF(stderr, "-end of input-(");
		}
		else
		{
			ANTLR3_FPRINTF(stderr, "-unknown source-(");
		}
	}
	else
	{
		ftext = ex->streamName->to8(ex->streamName);
		ANTLR3_FPRINTF(stderr, "%s(", ftext->chars);
	}

	// Next comes the line number
	//

	ANTLR3_FPRINTF(stderr, "%d) ", recognizer->state->exception->line);
	ANTLR3_FPRINTF(stderr, " : error %d : %s", 
										recognizer->state->exception->type,
					(pANTLR3_UINT8)	   (recognizer->state->exception->message));


	// How we determine the next piece is dependent on which thing raised the
	// error.
	//
	switch	(recognizer->type)
	{
	case	ANTLR3_TYPE_PARSER:

		// Prepare the knowledge we know we have
		//
		parser	    = (pANTLR3_PARSER) (recognizer->super);
		tparser	    = NULL;
		is			= parser->tstream->istream;
		theToken    = (pANTLR3_COMMON_TOKEN)(recognizer->state->exception->token);
		ttext	    = theToken->toString(theToken);

		ANTLR3_FPRINTF(stderr, ", at offset %d", recognizer->state->exception->charPositionInLine);
		if  (theToken != NULL)
		{
			if (theToken->type == ANTLR3_TOKEN_EOF)
			{
				ANTLR3_FPRINTF(stderr, ", at <EOF>");
			}
			else
			{
				// Guard against null text in a token
				//
				ANTLR3_FPRINTF(stderr, "\n    near %s\n    ", ttext == NULL ? (pANTLR3_UINT8)"<no text for the token>" : ttext->chars);
			}
		}
		break;

	case	ANTLR3_TYPE_TREE_PARSER:

		tparser		= (pANTLR3_TREE_PARSER) (recognizer->super);
		parser		= NULL;
		is			= tparser->ctnstream->tnstream->istream;
		theBaseTree	= (pANTLR3_BASE_TREE)(recognizer->state->exception->token);
		ttext		= theBaseTree->toStringTree(theBaseTree);

		if  (theBaseTree != NULL)
		{
			theCommonTree	= (pANTLR3_COMMON_TREE)	    theBaseTree->super;

			if	(theCommonTree != NULL)
			{
				theToken	= (pANTLR3_COMMON_TOKEN)    theBaseTree->getToken(theBaseTree);
			}
			ANTLR3_FPRINTF(stderr, ", at offset %d", theBaseTree->getCharPositionInLine(theBaseTree));
			ANTLR3_FPRINTF(stderr, ", near %s", ttext->chars);
		}
		break;

	default:

		ANTLR3_FPRINTF(stderr, "Base recognizer function displayRecognitionError called by unknown parser type - provide override for this function\n");
		return;
		break;
	}

	// Although this function should generally be provided by the implementation, this one
	// should be as helpful as possible for grammar developers and serve as an example
	// of what you can do with each exception type. In general, when you make up your
	// 'real' handler, you should debug the routine with all possible errors you expect
	// which will then let you be as specific as possible about all circumstances.
	//
	// Note that in the general case, errors thrown by tree parsers indicate a problem
	// with the output of the parser or with the tree grammar itself. The job of the parser
	// is to produce a perfect (in traversal terms) syntactically correct tree, so errors
	// at that stage should really be semantic errors that your own code determines and handles
	// in whatever way is appropriate.
	//
	switch  (ex->type)
	{
	case	ANTLR3_UNWANTED_TOKEN_EXCEPTION:

		// Indicates that the recognizer was fed a token which seesm to be
		// spurious input. We can detect this when the token that follows
		// this unwanted token would normally be part of the syntactically
		// correct stream. Then we can see that the token we are looking at
		// is just something that should not be there and throw this exception.
		//
		if	(tokenNames == NULL)
		{
			ANTLR3_FPRINTF(stderr, " : Extraneous input...");
		}
		else
		{
			if	(ex->expecting == ANTLR3_TOKEN_EOF)
			{
				ANTLR3_FPRINTF(stderr, " : Extraneous input - expected <EOF>\n");
			}
			else
			{
				ANTLR3_FPRINTF(stderr, " : Extraneous input - expected %s ...\n", tokenNames[ex->expecting]);
			}
		}
		break;

	case	ANTLR3_MISSING_TOKEN_EXCEPTION:

		// Indicates that the recognizer detected that the token we just
		// hit would be valid syntactically if preceeded by a particular 
		// token. Perhaps a missing ';' at line end or a missing ',' in an
		// expression list, and such like.
		//
		if	(tokenNames == NULL)
		{
			ANTLR3_FPRINTF(stderr, " : Missing token (%d)...\n", ex->expecting);
		}
		else
		{
			if	(ex->expecting == ANTLR3_TOKEN_EOF)
			{
				ANTLR3_FPRINTF(stderr, " : Missing <EOF>\n");
			}
			else
			{
				ANTLR3_FPRINTF(stderr, " : Missing %s \n", tokenNames[ex->expecting]);
			}
		}
		break;

	case	ANTLR3_RECOGNITION_EXCEPTION:

		// Indicates that the recognizer received a token
		// in the input that was not predicted. This is the basic exception type 
		// from which all others are derived. So we assume it was a syntax error.
		// You may get this if there are not more tokens and more are needed
		// to complete a parse for instance.
		//
		ANTLR3_FPRINTF(stderr, " : syntax error...\n");    
		break;

	case    ANTLR3_MISMATCHED_TOKEN_EXCEPTION:

		// We were expecting to see one thing and got another. This is the
		// most common error if we coudl not detect a missing or unwanted token.
		// Here you can spend your efforts to
		// derive more useful error messages based on the expected
		// token set and the last token and so on. The error following
		// bitmaps do a good job of reducing the set that we were looking
		// for down to something small. Knowing what you are parsing may be
		// able to allow you to be even more specific about an error.
		//
		if	(tokenNames == NULL)
		{
			ANTLR3_FPRINTF(stderr, " : syntax error...\n");
		}
		else
		{
			if	(ex->expecting == ANTLR3_TOKEN_EOF)
			{
				ANTLR3_FPRINTF(stderr, " : expected <EOF>\n");
			}
			else
			{
				ANTLR3_FPRINTF(stderr, " : expected %s ...\n", tokenNames[ex->expecting]);
			}
		}
		break;

	case	ANTLR3_NO_VIABLE_ALT_EXCEPTION:

		// We could not pick any alt decision from the input given
		// so god knows what happened - however when you examine your grammar,
		// you should. It means that at the point where the current token occurred
		// that the DFA indicates nowhere to go from here.
		//
		ANTLR3_FPRINTF(stderr, " : cannot match to any predicted input...\n");

		break;

	case	ANTLR3_MISMATCHED_SET_EXCEPTION:

		{
			ANTLR3_UINT32	  count;
			ANTLR3_UINT32	  bit;
			ANTLR3_UINT32	  size;
			ANTLR3_UINT32	  numbits;
			pANTLR3_BITSET	  errBits;

			// This means we were able to deal with one of a set of
			// possible tokens at this point, but we did not see any
			// member of that set.
			//
			ANTLR3_FPRINTF(stderr, " : unexpected input...\n  expected one of : ");

			// What tokens could we have accepted at this point in the
			// parse?
			//
			count   = 0;
			errBits = antlr3BitsetLoad		(ex->expectingSet);
			numbits = errBits->numBits		(errBits);
			size    = errBits->size			(errBits);

			if  (size > 0)
			{
				// However many tokens we could have dealt with here, it is usually
				// not useful to print ALL of the set here. I arbitrarily chose 8
				// here, but you should do whatever makes sense for you of course.
				// No token number 0, so look for bit 1 and on.
				//
				for	(bit = 1; bit < numbits && count < 8 && count < size; bit++)
				{
					// TODO: This doesn;t look right - should be asking if the bit is set!!
					//
					if  (tokenNames[bit])
					{
						ANTLR3_FPRINTF(stderr, "%s%s", count > 0 ? ", " : "", tokenNames[bit]); 
						count++;
					}
				}
				ANTLR3_FPRINTF(stderr, "\n");
			}
			else
			{
				ANTLR3_FPRINTF(stderr, "Actually dude, we didn't seem to be expecting anything here, or at least\n");
				ANTLR3_FPRINTF(stderr, "I could not work out what I was expecting, like so many of us these days!\n");
			}
		}
		break;

	case	ANTLR3_EARLY_EXIT_EXCEPTION:

		// We entered a loop requiring a number of token sequences
		// but found a token that ended that sequence earlier than
		// we should have done.
		//
		ANTLR3_FPRINTF(stderr, " : missing elements...\n");
		break;

	default:

		// We don't handle any other exceptions here, but you can
		// if you wish. If we get an exception that hits this point
		// then we are just going to report what we know about the
		// token.
		//
		ANTLR3_FPRINTF(stderr, " : syntax not recognized...\n");
		break;
	}

	// Here you have the token that was in error which if this is
	// the standard implementation will tell you the line and offset
	// and also record the address of the start of the line in the
	// input stream. You could therefore print the source line and so on.
	// Generally though, I would expect that your lexer/parser will keep
	// its own map of lines and source pointers or whatever as there
	// are a lot of specific things you need to know about the input
	// to do something like that.
	// Here is where you do it though :-).
	//
}

/// Return how many syntax errors were detected by this recognizer
///
*/