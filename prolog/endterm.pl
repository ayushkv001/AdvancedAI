endTerm(Tail, [Tail]).    % Base case: A single-element lists last element is itself.
endTerm(Tail, [_|Rest]) :- 
    endTerm(Tail, Rest).  % Recursive case: Traverse to the end of the list.
