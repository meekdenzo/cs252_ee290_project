% 
% function message = HD_functions
%   assignin('base','genBRandomHV', @genBRandomHV); 
%   assignin('base','projBRandomHV', @projBRandomHV); 
%   assignin('base','initItemMemories', @initItemMemories);
%   assignin('base','projItemMemeory', @projItemMemeory); 
%   assignin('base','computeNgramproj', @computeNgramproj); 
%   assignin('base','hdctrainproj', @hdctrainproj); 
%   assignin('base','hdcpredictproj', @hdcpredictproj);   
%   assignin('base','genRandomHV', @genRandomHV); 
%   assignin('base','downSampling', @downSampling);
%   assignin('base','genTrainData', @genTrainData);
%   assignin('base','lookupItemMemeory', @lookupItemMemeory);
%   assignin('base','hamming', @hamming);
%   message='Importing all HD functions';
% end
% 
% function [L_SAMPL_DATA, SAMPL_DATA] = genTrainData (data, labels, trainingFrac, order)
% %
% % DESCRIPTION   : generates a dataset to train the alorithm using a fraction of the input data 
% %
% % INPUTS:
% %   data        : input data
% %   labels      : input labels
% %   trainingFrac: the fraction of data we should use to output a training dataset
% %   order       : whether preserve the order of inputs (inorder) or randomly select
% %   donwSampRate: the rate or stride of downsampling
% % OUTPUTS:
% %   SAMPL_DATA  : dataset for training
% %   L_SAMPL_DATA: corresponding labels
% %    
% 
% 	rng('default');
%     rng(1);
%     L1 = find (labels == 1);
%     L2 = find (labels == 2);
%     L3 = find (labels == 3);
%     L4 = find (labels == 4);
%     L5 = find (labels == 5);
% 	L6 = find (labels == 6);
% 	L7 = find (labels == 7);
%    
%     L1 = L1 (1 : floor(length(L1) * trainingFrac));
%     L2 = L2 (1 : floor(length(L2) * trainingFrac));
%     L3 = L3 (1 : floor(length(L3) * trainingFrac));
%     L4 = L4 (1 : floor(length(L4) * trainingFrac));
%     L5 = L5 (1 : floor(length(L5) * trainingFrac));
% 	L6 = L6 (1 : floor(length(L6) * trainingFrac));
% 	L7 = L7 (1 : floor(length(L7) * trainingFrac));
%  
%     if order == 'inorder'
% 		Inx1 = 1:1:length(L1);
% 		Inx2 = 1:1:length(L2);
% 		Inx3 = 1:1:length(L3);
% 		Inx4 = 1:1:length(L4);
% 		Inx5 = 1:1:length(L5);
% 		Inx6 = 1:1:length(L6);
% 		Inx7 = 1:1:length(L7);
% 	else
% 		Inx1 = randperm (length(L1));
% 		Inx2 = randperm (length(L2));
% 		Inx3 = randperm (length(L3));
% 		Inx4 = randperm (length(L4));
% 		Inx5 = randperm (length(L5));
% 		Inx6 = randperm (length(L6));
% 		Inx7 = randperm (length(L7));
% 	end
%     
%     L_SAMPL_DATA = labels (L1(Inx1));
%     L_SAMPL_DATA = [L_SAMPL_DATA; (labels(L2(Inx2)))];
%     L_SAMPL_DATA = [L_SAMPL_DATA; (labels(L3(Inx3)))];
%     L_SAMPL_DATA = [L_SAMPL_DATA; (labels(L4(Inx4)))];
%     L_SAMPL_DATA = [L_SAMPL_DATA; (labels(L5(Inx5)))];
% 	L_SAMPL_DATA = [L_SAMPL_DATA; (labels(L6(Inx6)))];
% 	L_SAMPL_DATA = [L_SAMPL_DATA; (labels(L7(Inx7)))];
%     %L_SAMPL_DATA = L_SAMPL_DATA';
%     
%     SAMPL_DATA   = data (L1(Inx1), :);
%     SAMPL_DATA   = [SAMPL_DATA; (data(L2(Inx2), :))];
%     SAMPL_DATA   = [SAMPL_DATA; (data(L3(Inx3), :))];
%     SAMPL_DATA   = [SAMPL_DATA; (data(L4(Inx4), :))];
%     SAMPL_DATA   = [SAMPL_DATA; (data(L5(Inx5), :))];
% 	SAMPL_DATA   = [SAMPL_DATA; (data(L6(Inx6), :))];
% 	SAMPL_DATA   = [SAMPL_DATA; (data(L7(Inx7), :))];
% end
% 
% function [CiM, iM] = initItemMemories (D, MAXL, channels)
% %
% % DESCRIPTION   : initialize the item Memory  
% %
% % INPUTS:
% %   D           : Dimension of vectors
% %   MAXL        : Maximum amplitude of EMG signal
% %   channels    : Number of acquisition channels
% % OUTPUTS:
% %   iM          : item memory for IDs of channels
% %   CiM         : continious item memory for value of a channel
%  
%     % MAXL = 21;
% 	CiM = containers.Map ('KeyType','double','ValueType','any');
% 	iM  = containers.Map ('KeyType','double','ValueType','any');
%     rng('default');
%     rng(1);
%       
%     for i = 1 : channels
%         iM(i) = genRandomHV (D);
%     end
% 
%     initHV = genRandomHV (D);
% 	currentHV = initHV;
% 	randomIndex = randperm (D);
% 	
%     for i = 0:1:MAXL
%         CiM(i) = currentHV; 
%         SP = floor(D/2/MAXL);
% 		startInx = (i*SP) + 1;
% 		endInx = ((i+1)*SP) + 1;
% 		currentHV (randomIndex(startInx : endInx)) = not(currentHV (randomIndex(startInx: endInx)));
%     end
% end
% 
% function randomHV = lookupItemMemeory (itemMemory, rawKey, precision)
% %
% % DESCRIPTION   : recalls a vector from item Memory based on inputs
% %
% % INPUTS:
% %   itemMemory  : item memory
% %   rawKey      : the input key
% %   D           : Dimension of vectors
% %   precision   : precision used in quantization of input EMG signals
% %
% % OUTPUTS:
% %   randomHV    : return the related vector
% 
%  
%     key = int64 (rawKey * precision);
%   
%     if itemMemory.isKey (key) 
%         randomHV = itemMemory (key);
%     else
%         fprintf ('CANNOT FIND THIS KEY: %d\n', key);       
%     end
% end
% 
% function randomHV = genRandomHV(D)
% %
% % DESCRIPTION   : generate a random vector with zero mean 
% %
% % INPUTS:
% %   D           : Dimension of vectors
% % OUTPUTS:
% %   randomHV    : generated random vector
% 
%     if mod(D,2)
%         disp ('Dimension is odd!!');
%     else
%         randomIndex = randperm (D);
%         randomHV (randomIndex(1 : D/2)) = 1;
%         randomHV (randomIndex(D/2+1 : D)) = 0;
%        
%     end
% end
% 
% function [downSampledData, downSampledLabels] = downSampling (data, labels, donwSampRate)
% %
% % DESCRIPTION   : apply a downsampling to get rid of redundancy in signals 
% %
% % INPUTS:
% %   data        : input data
% %   labels      : input labels
% %   donwSampRate: the rate or stride of downsampling
% % OUTPUTS:
% %   downSampledData: downsampled data
% %   downSampledLabels: downsampled labels
% %    
% 	j = 1;
%     
%     for i = 1:donwSampRate:length(data(:,1))
%         
% 		downSampledData (j,:) = data(i, :);
% 		downSampledLabels (j) = labels(i);
%         j = j + 1;
%         
%     end
%     
%     downSampledLabels = downSampledLabels';
%     
% end
% 	
% function proj_m = projBRandomHV( D, F ,q)
% %   D: dim
% %   F: number of features
% %   q: sparsity
% 
%    proj_m=[]; 
%    if mod(D,2)
%         disp ('Dimension is odd!!');
%    else   
%     F_D=F*D;
%     probM=rand(F,D);
%     p_n1=(1-q)/2;
%     p_p1=p_n1;
% 
%     for k=1:F
%      for i=1:D
%          if probM(k,i)<p_n1     
%             proj_m(k,i)=-1;
%          else if (p_n1<=probM(k,i)) && (probM(k,i)<(q+p_n1))
%            proj_m(k,i)=0;   
%          else 
%            proj_m(k,i)=1; 
%          end
%      end
%      end 
%     end
%    end
%  
% end
% 
% function randomHV = projItemMemeory (projM, voffeature,ioffeature)
% %
% % INPUTS:
% %   projM	: random vector with {-1,0,+1}
% %   voffeature	: value of a feature
% %   ioffeature	: index of a feature
% % OUTPUTS:
% %   randomHV    : return the related vector
% 
%  
%         projV=projM(ioffeature,:);
%         h= voffeature.*projV;
% 
%     for i=1:length(h)
%       if h(i)>0
%         randomHV(i)=1;
%         else
%         randomHV(i)=0;
%        end
%     end
% 
% end
% 
% function Ngram = computeNgramproj (buffer, CiM, N, precision, iM, channels,projM)
% % 	DESCRIPTION: computes the N-gram
% % 	INPUTS:
% % 	buffer   :  data input
% % 	iM       :  Item Memory for IDs of the channels
% %   N        :  dimension of the N-gram
% %   precision:  precision used in quantization (no use)
% % 	CiM      :  Continious Item Memory for the values of a channel (no use)
% %   channels :  numeber of features
% % 	OUTPUTS:
% % 	Ngram    :  query hypervector
%     
%     %setup for first sample
%     chHV = projItemMemeory (projM, buffer(1, 1),1);
%     chHV = xor(chHV , iM(1));
%     v = chHV;
%     if channels>1    
%     for i = 2 : channels
%         chHV = projItemMemeory (projM, buffer(1, i), i);
%         chHV = xor(chHV , iM(i));
%         if i == 2
%             ch2HV=chHV; 
%         end
%         %create matrix of all channel final outputs to be bundled
%         v = [v; chHV];
%     end
%     
%     %don't understand why adding the xor of the final and 2nd channel
%     chHV = xor(chHV , ch2HV);
%     v = [v; chHV]; 
%     end
%     
%     %Add the other modalities
%     if channels==1
%     Ngram = v;
%     else
%     Ngram = mode(v);
%     end
%     
%     %Add later samples
%     for i = 2:1:N
%         %replicate for other modalities
%         chHV = projItemMemeory (projM, buffer(i, 1), 1);
%         chHV = xor(chHV , iM(1));
%         ch1HV = chHV;
%         %combine the other modalities
%         v = chHV;
%       if channels>1  
%         %replicate for other modalities
%         for j = 2 : channels
%             chHV = projItemMemeory (projM, buffer(i, j), j);
%             chHV = xor(chHV , iM(j));
%             if j == 2
%                 ch2HV=chHV; 
%             end
%             %combine the other modalities
%             v = [v; chHV];
%         end  
%         %combine other modalities for whatever this weird thing is
%         chHV = xor(chHV , ch2HV);
%         v = [v; chHV]; 
%       end
%       
%       if channels==1
%         record = v;          
%       else
%         record = mode(v); 
%       end
% 		Ngram = xor(circshift (Ngram, [1,1]) , record);
%            
%     end	 
%  
% end
%   
% function [numPat, AM] = hdctrainproj (labelTrainSet, trainSet, CiM, iM, D, N, precision, channels,projM ) 
% %
% % DESCRIPTION   : train an associative memory based on input training data
% %
% % INPUTS:
% %   labelTrainSet : training labels
% %   trainSet    : training data
% %   CiM         : cont. item memory (no use)
% %   iM          : item memory
% %   D           : Dimension of vectors
% %   N           : size of n-gram, i.e., window size
% %   precision   : precision used in quantization (no use)
% %
% % OUTPUTS:
% %   AM          : Trained associative memory
% %   numPat      : Number of stored patterns for each class of AM
% %
%  
% 	AM = containers.Map ('KeyType','double','ValueType','any');
% 
% 	
%     for label = 1:1:max(labelTrainSet)
%     	AM (label) = zeros (1,D);
% 	    numPat (label) = 0;
%     end
%     trainVecList=zeros (1,D);
%     i = 1;
%     label = labelTrainSet (1);
%     
%     while i < length(labelTrainSet)-N+1
%        	if labelTrainSet(i) == label  
%         %creates ngram for label    
%         %instead want to compute ngram which is fused, keep going if all
%         %of the labels for the modalities are the same. Once one changes,
%         %stop bundling and move onto the next sample for the next label for
%         %all modalities
% 	    ngram = computeNgramproj (trainSet (i : i+N-1,:), CiM, N, precision, iM, channels,projM);
%             trainVecList = [trainVecList ; ngram];
% 	        numPat (labelTrainSet (i+N-1)) = numPat (labelTrainSet (i+N-1)) + 1;
% 
%             i = i + 1;
%         else
%             %once you reach the end of that label, do majority count and
%             %set AM vector to be the bundled version of all the ngrams for
%             %that label, then move onto next label
%             trainVecList(1 , :) = 3;
%             AM (label) = mode (trainVecList);
%             label = labelTrainSet(i);
%             numPat (label) = 0;
%             trainVecList=zeros (1,D);
%     
%         end
%     end
%     l=floor(i+(N/2));
%     if l > length(labelTrainSet)
%        l= length(labelTrainSet);
%     end    
%     %wrap up the last training class
%     AM (labelTrainSet (l)) = mode (trainVecList);   
%     for label = 1:1:max(labelTrainSet)
% 		fprintf ('Class = %d \t sum = %.0f \t created \n', label, sum(AM(label)));
%     end
% end
% 
% function [accExcTrnz, accuracy, predicLabel, actualLabel] = hdcpredictproj (labelTestSet1, testSet1, labelTestSet2, testSet2,labelTestSet3, testSet3,AM, CiM, iM, D, N, precision, classes, channels1, channels2, channels3,projM1, projM2, projM3)
% %
% % DESCRIPTION   : test accuracy based on input testing data
% %
% % INPUTS:
% %   labelTestSet: testing labels
% %   testSet     : EMG test data
% %   AM          : Trained associative memory
% %   CiM         : Cont. item memory (no use)
% %   iM          : item memory
% %   D           : Dimension of vectors
% %   N           : size of n-gram, i.e., window size 
% %   precision   : precision used in quantization (no use)
% %
% % OUTPUTS:
% %   accuracy    : classification accuracy for all situations
% %   accExcTrnz  : classification accuracy excluding the transitions between gestutes
% %
% 	correct = 0;
%     numTests = 0;
% 	tranzError = 0;
% 	
%    
%     for i = 1:1:length(testSet1)-N+1
%         
% 		numTests = numTests + 1;
% 		actualLabel(i : i+N-1,:) = mode(labelTestSet1 (i : i+N-1));
%         %need to adjust hdcpredictproj here 
%         sigHV1 = computeNgramproj (testSet1 (i : i+N-1,:), CiM, N, precision, iM, channels1,projM1);
%         sigHV2 = computeNgramproj (testSet2 (i : i+N-1,:), CiM, N, precision, iM, channels2,projM2);
%    	    sigHV3 = computeNgramproj (testSet3 (i : i+N-1,:), CiM, N, precision, iM, channels3,projM3);
%         
%         sigHV=mode([sigHV1;sigHV2;sigHV3]);
% 
%    
% 	    predict_hamm = hamming(sigHV, AM, classes);
%         predicLabel(i : i+N-1) = predict_hamm;
%         
%         if predict_hamm == actualLabel(i)
% 			correct = correct + 1;
%         elseif labelTestSet1 (i) ~= labelTestSet1(i+N-1)
% 			tranzError = tranzError + 1;
%         end
%     end
%     
%     accuracy = correct / numTests;
%     accExcTrnz = (correct + tranzError) / numTests;
% 
%   
% end
% 
% function [predict_hamm] = hamming (q, aM, classes)
% %
% % DESCRIPTION       : computes the Hamming Distance and returns the prediction.
% %
% % INPUTS:
% %   q               : query hypervector
% %   AM              : Trained associative memory
% %
% % OUTPUTS:
% %   predict_hamm    : prediction 
% %
% 
%     sims = [];
%      
%     for j = 1 : classes
%         sims(j) = sum(xor(q,aM(j)));
%     end
%     
%     [~, indx]=min(sims');
%     predict_hamm=indx;
%      
% end
