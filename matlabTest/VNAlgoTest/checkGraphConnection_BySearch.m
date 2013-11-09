function isConnectedGraph = checkGraphConnection_BySearch(E)
% �ж�ͼ�Ƿ���ͨ�����������ķ���
% �μ�����:���������硷��4��--4.1С��:49-51
% ���������
%   E -- ��֧����
% ���������
%   isConnectedGraph -- �Ƿ���ͨ

% ��ͨ���ʼ��
CC = {};
CC{1}=[1];  % ����1����֧���뵽��1����ͨ����
for e = 2:size(E,1)
    if E(e,1) == -1
        continue;
    end
    
%     disp(sprintf('��%d����֧', e))
    u = E(e, 2);
    v = E(e, 3);
    i = findNodeInCC(E, CC, u);
    j = findNodeInCC(E, CC, v);
    
%     disp(sprintf('�ڵ�%dλ�ڵ�%d����ͨ��',u, i));
%     disp(sprintf('�ڵ�%dλ�ڵ�%d����ͨ��',v, j));
%     disp('�ϲ�֮ǰ:')
%     celldisp(CC)
    if i == -1 & j == -1           % �������κ���ͨ��
        len = length(CC)+1;
%         disp(sprintf('�½���ͨ��%d��������֧%d', len, e));
        CC{len} = [e];           % �½���ͨ��
    elseif i == -1 & j ~= -1       % u�������κ�һ����ͨ��; v���ڵ�j����ͨ��
%         disp(sprintf('�ϲ���֧%d����ͨ��%d', e, j));
        CC{j} = [CC{j} e];
    elseif i ~= -1 & j == -1       % u���ڵ�i����ͨ�飬�������κ�һ����ͨ��
%         disp(sprintf('�ϲ���֧%d����ͨ��%d', e, i));
%         disp([CC{i} e])
        CC{i} = [CC{i} e];
    elseif i == j                  % u��v�����ڵ�i����ͨ��
%         disp(sprintf('��֧%d��һ����ͨ��%d��', e, i));
        CC{i} = [CC{i} e];
    else                           % u���ڵ�i����ͨ�飬v���ڵ�j����ͨ��
%         disp(sprintf('��֧%d����2����ͨ��:%d��%d', i, j));
        CC{i} = [CC{i} CC{j}];
        CC(j)=[]; % ɾ����j����ͨ��
    end
%     disp('�ϲ�֮��:')
%     celldisp(CC)
end

% �ж���ͨ��ĸ������������ͨͼ����ô���յ���ͨ��Ŀ���Ӧ����1��
disp(length(CC))
% disp(size(CC))
% celldisp(CC)
for i=1:length(CC)
        disp(sprintf('��ͨ��%d��%s\n', i, mat2str(CC{i})));
end

isConnectedGraph = (length(CC)==1);


    function nodes = edgesToNodes(E, edges)
        % ����֧����ת���ɽڵ㼯��
        nodes = [];
        for ee = edges
            nodes = union(nodes, E(ee, 2:3));
        end
    end

    function index = findNodeInCC(E, CC, u)
        % ���ҽڵ�uλ���ĸ���ͨ����
        index = -1;
        for ii=1:length(CC)
            nodes = edgesToNodes(E, CC{ii}); % ��֧����ת���ɽڵ㼯��
%             disp(nodes);
            if ~isempty(find(nodes==u))
                index = ii;
                break;
            end
        end
    end

end