solution(Queens) :-
    permutation([1,2,3,4,5,6,7,8], Queens),
    safe(Queens).

safe([]).
safe([Queen|Others]) :-
    safe(Others),
    noattack(Queen, Others, 1).

noattack(_, [], _).
noattack(Y, [Y1|Ylist], Xdist) :-
    abs(Y1 - Y) =\= Xdist,
    Dist1 is Xdist + 1,
    noattack(Y, Ylist, Dist1).

% Helper predicates
permutation([], []).
permutation(List, [H|Perm]) :-
    del(H, List, Rest),
    permutation(Rest, Perm).

del(X, [X|T], T).
del(X, [H|T], [H|NT]) :-
    del(X, T, NT).
