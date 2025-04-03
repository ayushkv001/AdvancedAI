% Base case: An empty list or a single-element list is always a palindrome.
palindrome([]).
palindrome([_]).

% Recursive case: A list is a palindrome if its first and last elements are the same,
% and the sublist obtained by removing these elements is also a palindrome.
palindrome([H|T]) :-
    append(Mid, [H], T), % Split the list into Mid and H such that H is the last element of T.
    palindrome(Mid).
