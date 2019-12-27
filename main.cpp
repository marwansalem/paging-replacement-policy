#include <iostream>

#include <unordered_map>
#include <vector>

using namespace std;

unsigned long long number_of_frames = 0; // frames allocated to the process


int main()
{

    string policy_str;
    cin >> number_of_frames;
    int memory_frames[number_of_frames]; // contains the content of each frame

    unordered_map<int ,bool> present;
    for(unsigned int arr_idx = 0 ; arr_idx < number_of_frames ; ++arr_idx) {
        memory_frames[arr_idx] = -1; // to signify that they are empty

    }
    cin >> policy_str;

    int sequence_length = 0;
    int fault_counter = 0;
    vector<int> sequence;

    while(1) {
        int sc_page;
        scanf("%d", &sc_page);
        if( sc_page == -1) {
            break;
        }
        sequence.push_back(sc_page);
        sequence_length++;
    }

    if( !policy_str.compare("FIFO")) {
        int enqueue_ptr = 0;

        int current_size = 0;

        /*vector<int> sequence;
        while(1) {
            int page;
            scanf("%d", &page);
            if(page == -1) {
                break;
            }
            sequence.push_back(page);
            //present.insert(make_pair(page, false));
        }*/
        printf("Replacement Policy = FIFO\n");
        printf("-------------------------------------\n");
        printf("Page   Content of Frames\n");
        printf("----   -----------------\n");
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
                    present[page] = true;//mark the current page in sequence as present in memory
                    present[replaced_page] = false;//mark the page to be replaced as not present in memory
                    //basic fifo steps
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
        printf("Number of page faults = %d\n",fault_counter);
    } else if( !policy_str.compare("CLOCK")) {
        printf("Replacement Policy = CLOCK\n");
        printf("-------------------------------------\n");
        //vector<int> sequence;
        int clock_hand = 0;//clock hand , its an idx or pointer to
        // frame where a new page could be placed


        int current_size = 0;
        unordered_map<int, bool> present;
        int use_bits[number_of_frames];//use bit is part of clock algorithm
        //should be present in page table for each page in the process
        for(int idx = 0; idx < number_of_frames ; ++idx) {
            use_bits[idx] = 0;//initializing the bits to 0
        }
        printf("Page   Content of Frames\n");
        printf("----   -----------------\n");
        for(auto iter = sequence.begin(); iter != sequence.end(); ++iter) {
            bool fault_happened = false;
            int current_page = *iter;
            if(current_size < number_of_frames) { //transient state
                if(present.find(current_page) == present.end()) {
                    present.insert(make_pair(current_page, true));
                    memory_frames[clock_hand] = current_page;
                    use_bits[clock_hand] = 1;//mark the page as a newly placed (star it)
                    clock_hand++;
                    clock_hand %= number_of_frames;// the page table is circular
                    current_size++;
                }
            }else { // non transient state, when the allocated frames are used
                if(present.find(current_page) == present.end()) {//first encounter for this page
                    present.insert( make_pair(current_page, false));
                    //so lazily mark it as not present in memory
                }
                if(present.find(current_page) != present.end()) {// if it is not "first encounter"
                    if(!present[current_page]){//if the current page in sequence is in not present
                    fault_happened = true;//page fault occurred
                    fault_counter++;
                        while(1) { //clock policy algorithm
                            //loop until you find a frame that is not marked( no star on it)
                            if( !use_bits[clock_hand] ) {
                                //replace
                                int replaced_page = memory_frames[clock_hand];
                                present.find(replaced_page)->second = false;
                                memory_frames[clock_hand] = current_page;

                                present.find(current_page)->second = true;
                                use_bits[clock_hand] = 1;
                                clock_hand = (clock_hand + 1) %number_of_frames;//moving clock hand
                                break;
                            }
                            use_bits[clock_hand] = 0;//un-star the frame
                            clock_hand = (clock_hand + 1) %number_of_frames;//moving clock hand in circular buffer
                        }
                    } else {// should i refresh the present bit?
                        int idx = 0;
                        //loop until you find the frame in which currently referenced page is present
                        //another way would be to use a page table for process
                        //which could be a hashmap that maps each page with its current frame if it exists
                        while( memory_frames[idx++] != current_page);
                        //refresh or "re-star" a frame
                        // because it got referenced again
                        use_bits[idx-1] = 1;
                    }
                }

            }
            printf("%02d ", current_page);
            if(fault_happened) {
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
        printf("Number of page faults = %d\n",fault_counter);

    } else if( !policy_str.compare("LRU")) {
        int sequence_len = 0;

        unordered_map<int, int> page_time_present;
        // pages are keys, and the value is the most recent reference time stamp ,
        // or 0 if it is not present in memory
        auto seq_it = sequence.begin();
        while(seq_it != sequence.end()) {
            int page = *seq_it++;
            page_time_present.insert(make_pair(page, 0) );
        }
        printf("Replacement Policy = LRU\n");
        printf("-------------------------------------\n");
        printf("Page   Content of Frames\n");
        printf("----   -----------------\n");
        int time_idx = 1;
        int time_min_idx = 1; //could be used to find minimum in using better algorithm
        int time_line[sequence_length + 1]; // useless, but maybe could be used for a better algorithm for LRU (for the future)
        int current_size = 0;
        auto iter = sequence.begin();
        int transient_idx = 0;//to fill the process frames in order
        while(current_size < number_of_frames){
            int page = *iter++;// get next page in sequence

            if( page_time_present[page] == 0) {//page not present in memory so place it
                page_time_present[page] = time_idx;//update its timestamp
                time_line[time_idx++] = page;
                memory_frames[transient_idx++] = page;//placing the page in memory
                current_size++;
            } else {
                //debatable
                time_min_idx++;
                page_time_present[page] = time_idx; //major bugg caused by incrementing time_idx here
                time_line[time_idx++] = page;
            }

            printf("%02d     ", page);
            for(int idx = 0; idx < current_size; ++idx) {

                printf("%02d ", memory_frames[idx]);
            }
            printf("\n");
        }

        while(iter != sequence.end()) {
            bool fault_occurred = false;
            int page = *iter++; // I am not afraid of pointers, or iterators!

            if(page_time_present[page] != 0 ) { //page present; no fault
                page_time_present[page] = time_idx;//update the time stamp
                time_line[time_idx++] = page;
            } else { // page not in memory : replacing
                fault_counter++;
                fault_occurred = true;
                int replace_idx = 0;
                int min_time = time_idx+1;//definitely not last reference
                //loop until you find the least recent reference
                for(int frame_it = 0 ; frame_it < number_of_frames; ++frame_it) {
                    int page_ref_time = page_time_present[ memory_frames[frame_it]];
                    if(page_ref_time < min_time) {//finding frame with lowest reference timestamp
                        min_time = page_ref_time;
                        replace_idx = frame_it;
                    }
                }
                //replacing:
                page_time_present[memory_frames[replace_idx]] = 0; //mark the replaced page as not in memory
                memory_frames[replace_idx] = page; // place the current page in sequence
                page_time_present[page] = time_idx;//update its time stamp
                time_line[time_idx++] = page;

            }
            printf("%02d ", page);
            if(fault_occurred) {
                printf("F   ");
            } else {
                printf("    ");
            }
            for(int mem_idx = 0 ; mem_idx< number_of_frames ; ++mem_idx){
                printf("%02d ",memory_frames[mem_idx]);
            }
            printf("\n");

        }
        printf("-------------------------------------\n");
        printf("Number of page faults = %d\n",fault_counter);


    } else if ( !policy_str.compare("OPTIMAL")) {
        auto eiter = sequence.rbegin();
        // :D :D :D use reverse iterators
        //you can use a map of last reference for each page
        // or multimap of page that will be referenced no more

        printf("Replacement Policy = OPTIMAL\n");
        printf("-------------------------------------\n");
        printf("Page   Content of Frames\n");
        printf("----   -----------------\n");

        int current_size = 0;
        auto iter = sequence.begin();
        while(current_size < number_of_frames) {
            int page = *iter++;
            if(present.find(page) == present.end()) {
                present.insert(make_pair(page , true));
                memory_frames[current_size++] = page;
            }
            printf("%02d ", page);
            printf("    ");
            for(int mem_idx = 0 ; mem_idx< current_size ; ++mem_idx){
                printf("%02d ",memory_frames[mem_idx]);
            }
            printf("\n");
        }
        while (iter != sequence.end() ) {
            bool fault_occurred = false;

            int page = *iter++;
            if(present.find(page) == present.end()) {
                    present.insert(make_pair(page, false));
            }
            if(present[page] == false ) {// page fault_occurred
                fault_occurred = true;
                fault_counter++;
                present[page] = true;
                //find page to replace
                //optimal solution does not have to be implemented optimally :D
                int replace_idx = -1;
                int max_dist = -1;

                for(int process_page_idx = 0 ; process_page_idx < number_of_frames; ++process_page_idx){
                        auto following_seq_iter = iter;
                        int candidate_page = memory_frames[process_page_idx];
                        bool referenced = false;
                        int distance = 0;
                        int last_dist = 0;
                        while( following_seq_iter != sequence.end()) {
                            int future_page = *following_seq_iter++;
                            distance++;
                            if(memory_frames[process_page_idx] == future_page) {
                                //strong logical error
                                // the condition was prcoess_page_idx == future_page by mistake
                                // thank god I got it !
                                last_dist = distance;
                                referenced = true;
                            }
                        }
                        if(!referenced) {
                            // if a page is not referenced anymore,, then it should be replaced
                            replace_idx = process_page_idx;
                            break;
                        }

                        if(last_dist > max_dist) {
                            max_dist = last_dist;
                            replace_idx = process_page_idx;
                        }

                }
                present[ memory_frames[replace_idx] ] = false;
                memory_frames[replace_idx] = page;




            }
            printf("%02d ", page);
            if(fault_occurred) {
                printf("F   ");
            } else {
                printf("    ");
            }
            for(int mem_idx = 0 ; mem_idx< number_of_frames ; ++mem_idx){
                printf("%02d ",memory_frames[mem_idx]);
            }
            printf("\n");
        }
        printf("-------------------------------------\n");
        printf("Number of page faults = %d\n", fault_counter);
    }


    return 0;
}

