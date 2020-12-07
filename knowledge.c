/*
 * ICT1002 (C Language) Group Project.
 *
 * This file implements the chatbot's knowledge base.
 *
 * knowledge_get() retrieves the response to a question.
 * knowledge_put() inserts a new response to a question.
 * knowledge_read() reads the knowledge base from a file.
 * knowledge_reset() erases all of the knowledge.
 * knowledge_write() saves the knowledge base in a file.
 *
 * You may add helper functions as necessary.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "chat1002.h"

/*
 * Get the response to a question.
 *
 * Input:
 *   intent   - the question word
 *   entity   - the entity
 *   response - a buffer to receive the response
 *   n        - the maximum number of characters to write to the response buffer
 *
 * Returns:
 *   KB_OK, if a response was found for the intent and entity (the response is copied to the response buffer)
 *   KB_NOTFOUND, if no response could be found
 *   KB_INVALID, if 'intent' is not a recognised question word
 */
int knowledge_get(const char *intent, const char *entity, char *response, int n)
{
	// Create pointer to point to the current question. Must be not NULL.
	QUESTION *current_question_ptr = create_question("", "");

	// Search through all the intent in all_intents to get correct head_ptr
	// for current_question_ptr.
	for (int i = 0; i < MAX_NO_OF_INTENT - 1; i++)
	{
		// Check if intent match with intent in all_intent[i].
		if (compare_token(intent, all_intents[i].intent) == 0)
		{
			// If head_ptr is NULL, it meant that no question is inside
			// return KB_NOTFOUND.
			if (all_intents[i].head_ptr == NULL)
			{
				return KB_NOTFOUND;
			}
			current_question_ptr = all_intents[i].head_ptr;
			break;
		}
	}

	// Loop through link list of questions. Stop when response is found or
	// the ->next is NULL, meaning end of link list.
	while (1)
	{
		// Response is found, return found and set response up.
		if (compare_token(entity, current_question_ptr->entity) == 0)
		{
			strcpy(response, current_question_ptr->response);
			return KB_OK;
		}

		// No more question left, return not found.
		if (current_question_ptr->next == NULL)
		{
			return KB_NOTFOUND;
		}
		current_question_ptr = current_question_ptr->next;
	}

	// Free pointers used.
	return KB_INVALID;
}

/*
 * Insert a new response to a question. If a response already exists for the
 * given intent and entity, it will be overwritten. Otherwise, it will be added
 * to the knowledge base.
 *
 * Input:
 *   intent    - the question word
 *   entity    - the entity
 *   response  - the response for this question and entity
 *
 * Returns:
 *   KB_FOUND, if successful
 *   KB_NOMEM, if there was a memory allocation failure
 *   KB_INVALID, if the intent is not a valid question word
 */
int knowledge_put(const char *intent, const char *entity, const char *response)
{
	// Make sure that intent coming in is the same as intents in all_intents.
	int valid_intent = 0;
	for (int i = 0; i < MAX_NO_OF_INTENT - 1; i++)
	{
		if (compare_token(intent, all_intents[i].intent) == 0)
		{
			valid_intent = 1;
			break;
		}
	}
	if (valid_intent == 0)
	{
		return KB_INVALID;
	}

	// Create pointer to point to the new question with entity and response.
	QUESTION *new_question_ptr = create_question(entity, response);

	// intent_index keeps the index of the correct intent from all_intents.
	int intent_index = 999999;

	// Loop through all_intent and find correct index of intent.
	for (int i = 0; i < MAX_NO_OF_INTENT - 1; i++)
	{
		if (compare_token(all_intents[i].intent, intent) == 0)
		{
			intent_index = i;
			break;
		}
	}

	// If intent is not found, return invalid.
	if (intent_index == 999999)
	{
		// Free pointers used.
		free(new_question_ptr);
		return KB_INVALID;
	}

	// Create pointer to point to the current question. Must be not NULL.
	QUESTION *current_question_ptr = all_intents[intent_index].head_ptr;

	// Loop through link list of question of correct intent.
	while (1)
	{
		// If current_question_ptr is NULL, it mean it is the last question
		// hence add new_question as head and tail.
		if (current_question_ptr == NULL)
		{
			all_intents[intent_index].head_ptr = new_question_ptr;
			all_intents[intent_index].tail_ptr = new_question_ptr;
			break;
		}

		// If entity of current_question is the same as new entity, rewrite.
		else if (compare_token(current_question_ptr->entity, entity) == 0)
		{
			strcpy(current_question_ptr->response, "");
			strcpy(current_question_ptr->response, response);
			current_question_ptr = NULL;
			break;
		}

		else if (current_question_ptr->next != NULL)
		{
			current_question_ptr = current_question_ptr->next;
		}
		else
		{
			(*current_question_ptr).next = new_question_ptr;
			all_intents[intent_index].tail_ptr = new_question_ptr;
			break;
		}
	}

	return KB_OK;
}

/*
 * Read a knowledge base from a file.
 *
 * Input:
 *   f - the file
 *
 * Returns: the number of entity/response pairs successful read from the file
 */
int knowledge_read(FILE *f)
{
	char entity[MAX_ENTITY], response[MAX_RESPONSE];
	int lines_read = 0;

	// Create pointer to point to the buffer. Must be not NULL.
	char *buffer = (char *)malloc(MAX_RESPONSE + MAX_ENTITY + 1);
	if (buffer == NULL)
	{
		return KB_NOMEM;
	}

	// Create pointer to point to the current intent. Must be not NULL.
	char *current_intent = (char *)malloc(MAX_INTENT);
	if (buffer == NULL)
	{
		return KB_NOMEM;
	}
	strcpy(current_intent, "");

	// Read lines of file until the end.
	while (!feof(f))
	{
		// Clear buffer and populate it with line of file that is trimmed.
		strcpy(buffer, "");
		fgets(buffer, MAX_RESPONSE + MAX_ENTITY + 1, f);
		trim(buffer);

		// If the start is '[' and end is ']', it is an intent.
		if (buffer[0] == '[' && buffer[strlen(buffer) - 1] == ']')
		{
			// Create pointer to point to the temp_intent. Must be not NULL.
			char *temp_intent = (char *)malloc(strlen(buffer) - 2);
			if (buffer == NULL)
			{
				return KB_NOMEM;
			}

			// Clean intent of '[' and ']'.
			for (int i = 1; i < strlen(buffer) - 1; i++)
			{
				temp_intent[i - 1] = buffer[i];
			}

			// Put intent into current_intent.
			strcpy(current_intent, temp_intent);
		}

		if (strcmp(current_intent, "") != 0)
		{
			// If buffer contain '=', it is a entity/response.
			if (strstr(buffer, "="))
			{
				// Populate entity and response and put into knowledge.
				strcpy(entity, strtok(buffer, "="));
				strcpy(response, strtok(NULL, "="));
				knowledge_put(current_intent, entity, response);

				// Line read increment 1.
				lines_read++;
			}
		}
	}

	// Free pointers used.
	free(buffer);

	// Close file.
	fclose(f);

	// Return the number of responses read.
	return lines_read;
}

/*
 * Reset the knowledge base, removing all know entitities from all intents.
 */
void knowledge_reset()
{
	// Create pointer to point to the current question. Must be not NULL.
	QUESTION *current_question_ptr;
	if (current_question_ptr == NULL)
	{
		return;
	}

	// Create pointer to point to the next question. Must be not NULL.
	QUESTION *next_question_ptr;
	if (next_question_ptr == NULL)
	{
		return;
	}

	for (int i = 0; i < MAX_NO_OF_INTENT - 1; i++)
	{
		// Set head_ptr of intent as current question.
		current_question_ptr = all_intents[i].head_ptr;
		all_intents[i].head_ptr = NULL;

		// While current_question_ptr is not null, loop through to the next
		// question.
		while (current_question_ptr != NULL)
		{
			// Set the next question.
			next_question_ptr = current_question_ptr->next;
			current_question_ptr->next = NULL;
			// Free the current question.
			free(current_question_ptr);
			// Set the current question as the next question.
			current_question_ptr = next_question_ptr;
		}
	}

	// Free pointers used.
	free(current_question_ptr);
	free(next_question_ptr);
}

/*
 * Write the knowledge base to a file.
 *
 * Input:
 *   f - the file
 */
void knowledge_write(FILE *f)
{
	for (int i = 0; i < MAX_NO_OF_INTENT - 1; i++)
	{
		// If head_ptr and tail_ptr is not NULL, there is questions in intent.
		// Loop through then.
		if (all_intents[i].head_ptr != NULL && all_intents[i].tail_ptr != NULL)
		{
			// Write to file [intent_name] as header for all questions to come.
			fprintf(f, "\n[%s]\n", all_intents[i].intent);

			// Set current_question_ptr as head_ptr of intent.
			QUESTION *current_question_ptr = all_intents[i].head_ptr;

			//While the next question exist in the link list, write to file in:
			// entity=response format.
			while (1)
			{
				fprintf(f, "%s=", current_question_ptr->entity);
				fprintf(f, "%s\n", current_question_ptr->response);

				if (current_question_ptr->next == NULL)
				{
					break;
				}
				current_question_ptr = current_question_ptr->next;
			}
		}
	}
}

/*
 * Create a question pointer and return it.
 *
 * Input:
 *   entity - entity of the question
 *   response - response of the question
 */
QUESTION *create_question(const char *entity, const char *response)
{
	// Create pointer to point to the question. Must be not NULL.
	QUESTION *question_ptr = (QUESTION *)malloc(sizeof(QUESTION));
	if (question_ptr == NULL)
		return NULL;

	// Set up the question.
	question_ptr->next = NULL;
	strcpy(question_ptr->entity, entity);
	strcpy(question_ptr->response, response);

	return question_ptr;
}

/*
 * Create a smalltalk pointer and return it.
 *
 * Input:
 *   topic - topic of the smalltalk
 *   response - response of the smalltalk
 */
SMALLTALK *create_smalltalk(const char *topic, const char *response)
{
	// Create pointer to point to the smalltalk. Must be not NULL.
	SMALLTALK *smalltalk_ptr = (SMALLTALK *)malloc(sizeof(SMALLTALK));
	if (smalltalk_ptr == NULL)
		return NULL;

	// Set up the smalltalk.
	strcpy(smalltalk_ptr->topic, topic);
	strcpy(smalltalk_ptr->response, response);

	return smalltalk_ptr;
}

/*
 * Init the 5W1H intents into all_intents.
 *
 */
void init_knowledge()
{
	strcpy(all_intents[0].intent, "who");
	all_intents[0].head_ptr = NULL;
	all_intents[0].tail_ptr = NULL;
	strcpy(all_intents[1].intent, "what");
	all_intents[1].head_ptr = NULL;
	all_intents[1].tail_ptr = NULL;
	strcpy(all_intents[2].intent, "when");
	all_intents[2].head_ptr = NULL;
	all_intents[2].tail_ptr = NULL;
	strcpy(all_intents[3].intent, "where");
	all_intents[3].head_ptr = NULL;
	all_intents[3].tail_ptr = NULL;
	strcpy(all_intents[4].intent, "why");
	all_intents[4].head_ptr = NULL;
	all_intents[4].tail_ptr = NULL;
	strcpy(all_intents[5].intent, "how");
	all_intents[5].head_ptr = NULL;
	all_intents[5].tail_ptr = NULL;
}

char *ltrim(char *s)
{
	while (isspace(*s))
		s++;
	return s;
}

char *rtrim(char *s)
{
	char *back = s + strlen(s);
	while (isspace(*--back))
		;
	*(back + 1) = '\0';
	return s;
}

char *trim(char *s)
{
	return rtrim(ltrim(s));
}
