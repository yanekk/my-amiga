#include "vendor/unity.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
   /*
   struct List* myList = AllocMem(sizeof(struct List), MEMF_CLEAR);
   NewList(myList);

   MyNode node_1;
   node_1.node.ln_Name = "One";

   MyNode node_2;
   node_2.node.ln_Name = "Two";

   AddTail(myList, (struct Node*) &node_1);
   AddTail(myList, (struct Node*) &node_2);

   struct Node *node;
   for(node = myList->lh_Head; node->ln_Succ; node = node->ln_Succ) {
      printf("%s\n", node->ln_Name);
   }
   */
   struct NewGadget gadget;
   BeginLayout(&gadget);
   TEST_ASSERT_EQUAL(PADDING, gadget.ng_LeftEdge);
   TEST_ASSERT_EQUAL(PADDING * 2, gadget.ng_TopEdge);

   AddNextGadgetToLayout(&gadget, GADGET_HEIGHT);
   TEST_ASSERT_EQUAL(PADDING + GADGET_WIDTH + PADDING, gadget.ng_LeftEdge);
   TEST_ASSERT_EQUAL(PADDING * 2, gadget.ng_TopEdge);

   AddNextGadgetToLayout(&gadget, GADGET_HEIGHT);
   TEST_ASSERT_EQUAL(PADDING, gadget.ng_LeftEdge);
   TEST_ASSERT_EQUAL(PADDING * 2 + GADGET_HEIGHT + PADDING, gadget.ng_TopEdge);
}

int main(void)
{
   UnityBegin("test/test_acronym.c");

   RUN_TEST(test_node_creation);
   return UnityEnd();
}
