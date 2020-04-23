#ifndef PARSER_H
#define PARSER_H

/**
 * Parses specified string into a Pipeline struct
 *
 * @param string_to_parse	String that will be converted to Pipeline struct
 *
 * @return	Pipeline struct of the string argument
 *			PIPELINE_EMPTY if the string was empty
 *			PIPELINE_FAILED if enough memory could not be allocated
 */
PIPELINE *parse_pipeline(char *string_to_parse);

#endif /* PARSER_H */
