# Common

To run a program simply download the executable on a Windows machine and run it. I believe the rlutil library is specific to Windows, and I don't have a Mac so I couldn't generate a .app file and test it, but you're welcome to try. If for some reason the executable doesn't run, clone the repository and run compile.sh to rebuild them.

External libraries:
- [rlutil.h](https://github.com/tapio/rlutil), used for repositioning cursor within console.

Tooling language: C++11. I knew I wanted to program the game in C++ or Java just because I like working with OOP a lot more. JavaScript and Python have been enormously useful for web-based work, but all my practice with data structures and algorithms has been through using C++ (and occasionally Java) on [HackerRank](https://www.hackerrank.com). OOP languages are much faster and, though rigid, encourage me to think more about my design decisions. As an aside, not using a scripting language removed the subtle temptation to simply download an external library to execute some of the more difficult tasks. As for why I used C++11 instead of 14 or 17, it's just the version that's currently installed on my computer.

# Solitaire

I designed three specialized linear data structures for solitaire, two of which are based on the basic doubly-linked list and one of which is a pseudo-structure.

First is the [linked list](linked_list.h), which is used for representing the tableau. The root node is not displayed, but is used to separate covered cards from uncovered cards. Essentially, any card that comes before the root node is classified as covered, and any that comes after is uncovered. This made uncovering cards as simple as shifting the root's predecessor (oxymoronic, I know) one spot to the right.

Next is the [circular queue](circular_queue.h), which is used to represent the stock pile. There's no clever trick with this structure, I just needed the implementation because it seemed like the best way to represent the pile.

Finally is the pseudo-structure used for the foundation piles, which I've dubbed the [shallow stack](shallowstack.h). In terms of behavior it is similar to an ordinary stack, but I capitalize on the fact that every card in the foundation must be one value higher than the next element. Because of this behavior I was able to save on storage and represent the entire stack using a single element, and simply increment/decrement when pushing/popping.

The shallow stack may seem like an unnecessary complication, since I could've used a simple card without wrapping it in a data structure. However, as seen in the [collection](collection.h) superclass, my goal was to abstract the behavior of each of the three types of piles in order to simplify the logic. This way I don't have to maintain the stock, foundation, and tableau separately, nor do I have to use a series of if/else statement when deciding which method to call.

# Poker

The rules are for basic Hold'em poker, with each player starting at 2000 dollars. The player and CPU each get two cards, then five cards are placed in the river. There is a buy-in followed by single-round betting with 0, 3, 4, and 5 river cards exposed. If either player folds, the other is not compelled to reveal their cards and the round ends there. After the final round of betting all cards and revealed and the player with the best five cards wins. The possible hands (from most to least valuable) are:

- **Straight flush**   Five cards of the same suit with sequential ranks. *ties are won by the straight that ends at the highest rank*
- **Four of a kind**   Four cards have the same rank. *ties are won by the set of four with the higher rank*
- **Full house**       Three cards have one rank and the other two have another. *ties are won by the set of threes with the higher rank*
- **Flush**            Five cards of the same suit. *ties are won by comparing the highest ranked card in each hand*
- **Straight**         Five cards with sequential ranks. *ties are won by the straight that ends at the highest rank*
- **Three of a kind**  Three cards have the same rank. *ties are won by the set of three with the higher rank*
- **Two pair**         Two cards have one rank, two cards have another, and the last has yet another. *ties are won by comparing the rank of the highest pair, then the rank of the next highest pair, then the rank of the remaining card*
- **Pair**             Two cards have one rank and the remaining three have different ranks. *ties are won by comparing the rank of the pair, then the ranks of the remaining cards*
- **High card**        Any other hand. *ties are won by comparing the highest ranked card in each hand, then the next highest, and so on*

Aces may be high or low for straights, but are high when evaluating rank for tie breaks. If the tie break in question is a straight or a straight flush, the ace assumes a high or low value depending on its position in the straight.

To evaluate hands I tracked a 13-bit representation of each suit in the hand. The nth bit from the right represents a card with rank n. I also kept track of the combined bitwise OR and bitwise XOR of the suit representations, the number of suits, and the frequency of each rank. This system is not necessarily the most efficient in terms of building a new hand, but is very quick for toggling individual cards, letting me rapidly evaluate a series of hands with small hamming distances. Using this info it's relatively simple to classify hands using bit operations, as follows:

- **Straight flush**   One suit present, OR is equal to 31 * (OR & -OR). Special case (ace low): OR = 0x100F
- **Four of a kind**   Two ranks present, bitwise AND of all four suit representations is nonzero.
- **Full house**       Two ranks present, four-of-a-kind test failed.
- **Flush**            One rank present.
- **Straight**         OR is equal to 31 * (OR & -OR). Special case (ace low): OR = 0x100F.
- **Three of a kind**  Three ranks present, three odd frequencies.
- **Two pair**         Three ranks present, one odd frequency.
- **Pair**             Four ranks present.
- **High card**        All other hands.

Each hand requires up to three tiers of identification. First is actual hand value (a number 0-8 assigned using the above). Then comes the primary and secondary rank comparison (ex: for a two pair, we need to compare the ranks of the pairs while disregarding the kicker, then compare the kicker rank). In most cases this can be accomplished by concatenating some combination of OR (all ranks in order), XOR (odd-frequency ranks in order), and (OR^XOR) (even-frequency ranks in order). The only exception to this would be the ace-low straight, which can be hard coded, and the 3/4 of a kind, which can be calculated using bitwise and operations.

The computer uses two factors when deciding its move. The first is purely mathematical, running 100,000 simulations to calculate the probability of victory. Dividing this by the pot odds (investment after call) divided by (total pot value) yields the rate of return, which splits the decision making into tiers. Low return rates increase the chance of folding while high return rates increase the chance of raising. When raising, the computer attempts to bring the return rate reasonably close to 1, although it fuzzes the number into a range from 0.8 to 1.2 in order to keep the user from calculating victory probabilities.

The second factor in deciding the computer's move is based on the conveyed return rate of the user. After a round ends, if the computer has been shown the user's cards, it evaluates the user's move for each round of betting. At each stage it is easy to calculate the rate of return which the user wanted to convey. By running simulations on the game state, the computer learns to associate these conveyed rates of return (stratified into buckets of size 0.1) with the user's actual probabilities of winning. This probability is used to weight the probability calculated from simulations.
