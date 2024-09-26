# Voting/electoral systems

Implementations of voting/electoral systems

1. Simple Plurality
2. Single Transferable Vote

# Note on code quality
Code is **atrocious** currently, in a rush to make one of these usable.
Update incoming soon.

# Simple Plurality

There are N candidates, M voters and K seats (winners).
Each voter can vote for (at most) one candidate.

## Result

The winners are the K candidates with the most votes.
It can happen that a tie results in the final winners being undetermined.
One action that can be performed to break the tie is having a second round between the tied candidates.

# Single Transferable Vote

There are N candidates, M voters and K seats (winners).
Each candidate is marked hopeful at the start, but can be marked elected or eliminated as the process goes on.
Each voter can order the candidates in the order they like.
Voters are allowed to stop ordering after a certain point, which effectively states they'd rather leave the seat empty.
Votes initially are transfered to the first preference listed within the vote.
As candidates get elected/eliminated, the
The Droop quota is used in this implementation.
This means a candidate that has received at least Q = floor(M / (K + 1)) + 1 will be elected.
The votes are then processed in rounds.
The first round is an election round.

## Election rounds

Look at the candidate with most votes, call it C.
Suppose C has X votes.

If X >= Q then C is elected and we have S = X - Q surplus votes for C.
Randomly choose S out of the X votes and for each of those, transfer it to the next hopeful candidate in the order.
Then move to the next round, which will be an election round.

If X < Q then C did not meet the quota and we move on to the next round, which will be an elimination round.

## Elimination rounds

Look at the candidate with the least votes, call it C.
For each vote for C, transfer the vote to the next hopeful candidate in the order.

## Result

After all candidates have been either Elected or Eliminated, we will have W winners, where W <= K.
This means some seats may be left empty if voters do not give a full ordering of all candidates.

