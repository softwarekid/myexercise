function [V_V, V_E, V_from, V_to] = dealWithMultiSSNetwork(V, E, from, to)
% 处理多源多汇网络，采用添加虚节点和虚分支的方法
% 输出参数：
%   V_V        --     添加了虚节点之后的节点集合
%   V_E        --     添加了虚分支之后的分支集合
%   V_from     --     虚拟的源点
%   V_to       --     虚拟的汇点

m = length(V); % 节点数
n = size(E,1); % 分支数

V_V = V;
V_E = E;
V_from = from(1);
V_to = to(1);

% 如果存在x个源点，则添加一个虚拟的源点和x个虚拟分支
if length(from)>1
    m = m+1;
    V_from = m;       % 虚拟的源点
    V_V(V_from) = -1; % 对应的真实节点编号为虚拟的-1
      
    for u = from
        V_E(end+1, 1:end) = [-1 V_from u];
    end
end

% 如果存在x个汇点，则添加一个虚拟的汇点和x个虚拟的分支
if length(to)>1
    m = m+1;
    V_to = m;        % 虚拟的汇点
    V_V(V_to) = -1;  % 对应的真实节点编号为虚拟的-1
    
    for u = to
        V_E(end+1, 1:end)=[-1 u V_to];
    end
end

end