// Simple Arithmetics Grammar
// ==========================
//
// Accepts expressions like "2 * (3 + 4)" and computes their value.

Assignments = h:Assignment t:(_ Assignment)* _
{ 

let retval = t.reduce((tx, x) => tx.concat(x[1].rules), h.rules).map((x) => "\"" + x[0] + "\" -> \"" + x[1] + "\"").join("\n");
console.log(retval)
return retval;
}
Assignment = l:LHS _ "=" _ r:RHS
{ let node = l.node + "=" + r.node; return { rules: r.rules.concat([[l.node, node], [node, r.node]]), node: node }; }
LHS = Identifier / StaticArrayLoc
StaticArrayLoc = i:Identifier l:("[" [0-9]+ "]") { return {rules: [], node: i.node + l.join("")}; }
Identifier = p1:("pablo.")?p2:[a-zA-Z_0-9]+ { return {rules: [], node: (p1 ? p1 : "") + p2.join("")}; }
// ~ & ^ |
RHS = RHS_11 / RHS_12 / RHS_13 / RHS_14
RHS_11 = h:RHS_12 t:(_ "|" _ RHS_12)*
{
  var t = t.map((tx) => tx[3]);
  var node = t.reduce((tx, x) => tx + "|" + x.node, h.node);
  var rules = t.length == 0 ? [] : [h].concat(t).map((x) => [node, x.node]);
  return { rules: t.reduce((tx, x) => tx.concat(x.rules), h.rules || []).concat(rules), node: node};
}
RHS_12 = h:RHS_13 t:(_ "^" _ r:RHS_13)* 
{
  var t = t.map((tx) => tx[3]);
  var node = t.reduce((tx, x) => tx + "^" + x.node, h.node);
  var rules = t.length == 0 ? [] : [h].concat(t).map((x) => [node, x.node]);
  return { rules: t.reduce((tx, x) => tx.concat(x.rules), h.rules || []).concat(rules), node: node};
}
RHS_13 = h:RHS_14 t:(_ "&" _ r:RHS_14)*
{
  var t = t.map((tx) => tx[3]);
  var node = t.reduce((tx, x) => tx + "&" + x.node, h.node);
  var rules = t.length == 0 ? [] : [h].concat(t).map((x) => [node, x.node]);
  return { rules: t.reduce((tx, x) => tx.concat(x.rules), h.rules || []).concat(rules), node: node};
}
RHS_14 = "(" r:RHS ")" { let node = "(" + r.node + ")"; return { rules: r.rules.concat([[node, r.node]]), node: node}; }
/ "~" r:RHS { let node = "~" + r.node; return { rules: r.rules.concat([[node, r.node]]), node: node }; }
/ f:Identifier _ "(" a1:RHS _ "," _ a2:RHS ")"
 { let node = f.node + "(" + a1.node + "," + a2.node + ")"; return {
 rules: a1.rules.concat(a2.rules).concat([[node, a1.node], [node, a2.node]]), node: node }; }
/ StaticArrayLoc
/ Identifier

_ "whitespace"
  = [ \t\n\r]*
