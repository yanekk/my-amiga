#include "vendor/unity.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#include <clib/gadtools_protos.h>
#include "../src/layout.h"

typedef struct MyNode {
   struct Node node;
   int data;
} MyNode;

void setUp(void)
{
}

void tearDown(void)
{

}

static void test_node_creation(void)
{
   struct Layout* layout = CreateLayout();
   int buttonId = AddGadgetToLayout(layout, BUTTON_KIND, "First Button", PLACETEXT_IN, GA_ActivateKey, 123, GA_BackFill, 321, TAG_END);
   int secondButtonId = AddGadgetToLayout(layout, BUTTON_KIND, "Second Button", PLACETEXT_IN, GA_ActivateKey, 123, GA_BackFill, 321, TAG_END);
   TEST_ASSERT_EQUAL(1, buttonId);
   TEST_ASSERT_EQUAL(2, secondButtonId);
   FreeLayout(layout);
}

int main(void)
{
   UnityBegin("test/test_acronym.c");

   RUN_TEST(test_node_creation);
   return UnityEnd();
}
