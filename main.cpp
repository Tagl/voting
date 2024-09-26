#include <algorithm>
#include <iomanip>
#include <iostream>
#include <random>
#include <ranges>
#include <vector>

using namespace std;
using int_t = int64_t;

struct candidate_t {
    int_t id;
    string name;
};

void simple_plurality() {
    cout << "Number of winners: ";
    int_t number_of_winners;
    cin >> number_of_winners;

    cout << "Number of candidates: ";
    int_t number_of_candidates;
    cin >> number_of_candidates;

    int_t slots{ number_of_candidates + 1};

    vector<candidate_t> candidates(slots);
    
    candidates[0] = {0, "INVALID"};
    for (int_t i = 1; i < slots; i++) {
        auto& candidate{ candidates[i] };
        candidate.id = i;
        cout << "Candidate " << candidate.id << ": ";
        cin >> candidate.name;
    }

    cout << "Number of ballots: ";
    int_t number_of_ballots;
    cin >> number_of_ballots;

    vector<int_t> vote_count(slots);
    for (int_t i = 1; i <= number_of_ballots; i++) {
        cout << "Ballot " << i << ": ";
        int_t vote;
        if (!(cin >> vote) || vote < 1 || number_of_candidates < vote) {
           vote = 0;
        }
        vote_count[vote]++;
    }

    ranges::sort(candidates, [&vote_count](const candidate_t& a, const candidate_t& b) {
        return vote_count[b.id] < vote_count[a.id];
    });

    cout << setprecision(2) << fixed << endl;
    for (int_t i = 0; i < slots; i++) {
        const auto& candidate{ candidates[i] };
        const auto tally{ vote_count[candidate.id] };
        const auto percentage{ static_cast<double>(tally) / number_of_ballots };
        cout << i + 1 << ": (" << candidate.id << ") " << candidate.name 
            << " received " << tally << " votes, or " << percentage << "%" << endl;
    }

    int_t place = 1;
    int_t remaining_winners = number_of_winners;
    for (int_t start = 0, end = start; start + 1 < slots && remaining_winners > 0; start = end + 1) {
        while (vote_count[candidates[start].id] == vote_count[candidates[end + 1].id]) {
            end++;
        }
        int_t count = end - start + 1;
        if (count > remaining_winners) {
            cout << "Tie for last " << remaining_winners << " spots between " << count << " candidates:" << endl;
            for (int_t i = start; i <= end; i++) {
                cout << "(" << candidates[i].id << ") " << candidates[i].name << endl;
            }
            remaining_winners = 0;
        }
        else {
            for (int_t i = start; i <= end; i++) {
                cout << "Winner " << place << ": " << "(" << candidates[i].id << ") " << candidates[i].name << endl;
            }
            remaining_winners -= count;
        }
    }
}



struct stv_t {
    int_t active_index;
    vector<int_t> order;
    stv_t(int_t n) : active_index(0), order(n, 0) {
    }
    struct transfer_t {
        int_t old, current;
    };
    int_t get() const {
        return order[active_index];
    }
    transfer_t transfer() {
        const int_t old{ get() };
        if (active_index + 1 == ssize(order)) {
            return { old, old };
        }
        active_index++;
        return { old, get() };
    }
};

void single_transferable_vote() {
    cout << "Number of winners: ";
    int_t number_of_winners;
    cin >> number_of_winners;

    cout << "Number of candidates: ";
    int_t number_of_candidates;
    cin >> number_of_candidates;

    int_t slots{ number_of_candidates + 1};

    vector<candidate_t> candidates(slots);
    
    candidates[0] = {0, "EMPTY"};
    for (int_t i = 1; i < slots; i++) {
        auto& candidate{ candidates[i] };
        candidate.id = i;
        cout << "Candidate " << candidate.id << ": ";
        cin >> candidate.name;
    }

    cout << "Number of ballots: ";
    int_t number_of_ballots;
    cin >> number_of_ballots;
    
    vector<stv_t> votes(number_of_ballots, number_of_candidates);
    vector<int_t> vote_count(slots, 0);

    for (int_t i = 1; i <= number_of_ballots; i++) {
        auto& vote{ votes[i - 1] };
        cout << "Ballot " << i << ": ";
        for (auto& v : vote.order) {
            if (!(cin >> v) || v < 1 || number_of_candidates < v) {
               v = 0;
            }
        }
        vote_count[vote.get()]++;
    }

    int_t droop_quota = number_of_ballots / (number_of_winners + 1) + 1;

    int_t round{ 1 };
    bool is_elimination_round{ false };
    int_t remaining_candidates { number_of_candidates };
    int_t remaining_winners{ number_of_winners };
    constexpr int_t hopeful{ 0 };
    constexpr int_t elected{ 1 };
    constexpr int_t eliminated{ -1 };
    vector<int_t> status(slots, hopeful);
    std::mt19937 rng{ std::random_device{}() };
    vector<int_t> winners;
    while (remaining_candidates > 0) {
        cout << "Starting round " << round << (is_elimination_round ? " (Elimination round)" : "") << endl;
        cout << "Current vote count is:" << endl;
        for (const auto& c : candidates) {
            cout << "(" << c.id << ") " << c.name << ": " << vote_count[c.id] << " (" << static_cast<double>(vote_count[c.id]) / number_of_ballots << ") ";
            if (status[c.id] == elected) {
                cout << "[ELECTED]";
            }
            else if (status[c.id] == eliminated) {
                cout << "[ELIMINATED]";
            }
            cout << endl;
        }
        if (is_elimination_round) {
            cout << "Finding hopeful candidate to eliminate" << endl;
            int_t min_ind = 1;
            while (status[min_ind] != hopeful) {
                min_ind++;
            }
            for (int_t i = 1; i < slots; i++) {
                if (status[i] == hopeful && vote_count[i] < vote_count[min_ind]) {
                    min_ind = i;
                }
            }

            cout << "Hopeful candidate with minimum votes is (" << min_ind << ") " << candidates[min_ind].name << endl;
            cout << "Transfering votes" << endl;
            int_t transfer_count{ 0 };
            for (auto& vote : votes) {
                if (vote.get() == min_ind) {
                    vote_count[vote.get()]--;
                    while (status[vote.transfer().current] != hopeful);
                    vote_count[vote.get()]++;
                    transfer_count++;
                }
            }
            cout << transfer_count << " votes transfered" << endl;
            
            status[min_ind] = eliminated;
            remaining_candidates--;
            cout << "Candidate marked as eliminated" << endl;

            is_elimination_round = false;
        }
        else {
            cout << "Searching for candidate to mark as elected" << endl;
            
            int_t max_ind = 1;
            while (status[max_ind] != hopeful) {
                max_ind++;
            }
            for (int_t i = 1; i < slots; i++) {
                if (status[i] == hopeful && vote_count[i] > vote_count[max_ind]) {
                    max_ind = i;
                }
            }
            
            cout << "Hopeful candidate with maximum votes is (" << max_ind << ") " << candidates[max_ind].name << endl;

            if (vote_count[max_ind] < droop_quota) {
                cout << "Candidate does not meet the quota as " << vote_count[max_ind] << " < " << droop_quota << endl;
                is_elimination_round = true;
            }
            else {
                int_t surplus_count{ vote_count[max_ind] - droop_quota };
                cout << "Candidate meets the quota with " << surplus_count << " surplus votes as " << vote_count[max_ind] << " >= " << droop_quota << endl;
                cout << "Collecting candidate votes" << endl;
                vector<int_t> votes_for_candidate;
                for (int_t i = 0; i < number_of_ballots; i++) {
                    if (votes[i].get() == max_ind) {
                        votes_for_candidate.push_back(i);
                    }
                }
                cout << "Randomly selecting " << surplus_count << " votes to transfer out of total " << ssize(votes_for_candidate) << endl;
                vector<int_t> surplus_votes;
                std::sample(votes_for_candidate.begin(), votes_for_candidate.end(), std::back_inserter(surplus_votes), surplus_count, rng);

                cout << "Transfering votes" << endl;
                int_t transfer_count{ 0 };
                for (auto& i : surplus_votes) {
                    auto& vote{ votes[i] };
                    if (vote.get() == max_ind) {
                        vote_count[vote.get()]--;
                        vote.transfer();
                        vote_count[vote.get()]++;
                        transfer_count++;
                    }
                }
                cout << transfer_count << " votes transfered" << endl;
                
                status[max_ind] = elected;
                winners.push_back(max_ind);
                remaining_winners--;
                remaining_candidates--;
                cout << "Candidate marked as elected" << endl;
                is_elimination_round = false;
            }
        }
        cout << "Round " << round << " ended" << endl << endl;
        round++;
    }

    cout << "Current vote count is:" << endl;
    for (const auto& c : candidates) {
        cout << "(" << c.id << ") " << c.name << ": " << vote_count[c.id] << " (" << static_cast<double>(vote_count[c.id]) / number_of_ballots << ") ";
        if (status[c.id] == elected) {
            cout << "[ELECTED]";
        }
        else if (status[c.id] == eliminated) {
            cout << "[ELIMINATED]";
        }
        cout << endl;
    }
    cout << endl;

    while (ssize(winners) < number_of_winners) {
        winners.push_back(0); 
    }
    cout << "The winners (in order) are:" << endl;
    for (int_t i : winners) {
        auto& c{ candidates[i] };
        cout << "(" << c.id << ") " << c.name << endl;

    }
}

void schulze() {
    cout << "Unimplemented so far!" << endl;
}

int main() {
    cout << "1. Simple Plurality" << endl;
    cout << "2. Single Transferable Vote" << endl;
    cout << "3. Schulze" << endl;
    cout << "Select method: ";

    int_t choice;
    if (!(cin >> choice)) {
        cout << "Invalid choice!" << endl;
        return 0;
    }
    
    switch (choice) {
        case 1:
            simple_plurality();
            break;
        case 2:
            single_transferable_vote();
            break;
        case 3:
            schulze();
            break;
        default:
            cout << "Invalid choice!" << endl;
            break;
    }

    return 0;
}
