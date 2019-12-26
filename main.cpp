#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
using namespace std;

unsigned long long number_of_frames = 0; // frames allocated to the process
int *mem_ptr = NULL;
bool *pres_ptr = NULL;
void fifo();
int main()
{

    string policy_str;
    cin >> number_of_frames;
    int memory_frames[number_of_frames]; // contains the content of each frame
    bool page_present[number_of_frames]; // contains booleans representing whether a page is present in the memory or not
    unordered_map<int ,bool> present;
    mem_ptr = memory_frames;
    pres_ptr = page_present;
    for(unsigned int arr_idx = 0 ; arr_idx < number_of_frames ; ++arr_idx) {
        memory_frames[arr_idx] = -1; // to signify that they are empty
        page_present[arr_idx] = false;
    }
    cin >> policy_str;
    int sequence_length = 0;

    if( !policy_str.compare("FIFO")) {
        int enqueue_ptr = 0;
        int dequeue_ptr = 0;
        int current_size = 0;
        int fault_counter = 0;
        vector<int> sequence;
        while(1) {
            int page;
            scanf("%d", &page);
            if(page == -1) {
                break;
            }
            sequence.push_back(page);
            //present.insert(make_pair(page, false));
        }
        printf("Replacement Policy = FIFO\n");
        printf("-------------------------------------\n");
        for(auto iter = sequence.begin(); iter != sequence.end(); ++iter ){
            bool fault_occurred = false;
            int page = *iter;


            if( current_size < number_of_frames ) {

                if( present.find(page) == present.end() ) {
                    memory_frames[enqueue_ptr] = page;
                    present.insert( make_pair(page, true));
                    current_size++;
                    enqueue_ptr = (enqueue_ptr+1) % number_of_frames;
                }
            } else {
                if(present.find(page) == present.end()){// first time to see this page
                    present.insert(make_pair(page, false));//so it is not present yet in memory
                }
                if( !(present.find(page)->second) ){
                    fault_occurred = true;
                    fault_counter++;
                    int replaced_page = memory_frames[enqueue_ptr];
                    present[page] = true;
                    present[replaced_page] = false;
                    memory_frames[enqueue_ptr] = page;
                    enqueue_ptr = (enqueue_ptr+1) % number_of_frames;

                }

            }

            printf("%02d ",page);
            fault_occurred = fault_occurred && (current_size == number_of_frames);
            //page faults do not count if the process frames are not 'filled'
            if(fault_occurred) {
                printf("F   ");
            } else {
                printf("    ");
            }
            for(int idx = 0; idx < current_size; ++idx) {
                printf("%02d ", memory_frames[idx]);
            }
            printf("\n");
        }
        printf("-------------------------------------\n");
        printf("Number of page faults = %d",fault_counter);
    } else if( !policy_str.compare("OPTIMAL")) {

    }


    return 0;
}

void fifo(){
    int scanned_page;

    queue<int> q;
    while(1) {
        scanf("%d", &scanned_page);


    }

}
