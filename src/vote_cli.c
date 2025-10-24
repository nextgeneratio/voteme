#include <stdio.h>
#include "voting-interface.h"
#include "data_errors.h"

int main(void)
{
    printf("\nWelcome to VoteMe - Voter Interface\n");
    printf("-----------------------------------\n\n");
    int rc = vote_for_candidate_interactive();
    if (rc == DATA_SUCCESS)
    {
        printf("\n✅ Vote recorded successfully.\n");
        return 0;
    }
    else
    {
        printf("\n❌ Vote failed (code %d).\n", rc);
        return 1;
    }
}
