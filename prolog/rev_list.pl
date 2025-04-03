reverse_list([], []).
reverse_list([H|T], Rev) :-
 reverse_list(T, RevT),
 append(RevT, [H], Rev).