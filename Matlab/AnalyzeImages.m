function [diceValues, densityValues, TPVFVal, PrecisionVal, FPVFVal] = AnalyzeImages(testImgs, groundTruths, nfiles, print)
% Calculates the dice, density, TPVF, FPVF and Precision
% inputs are two cell arrays containing 1-dimensional logical images


% Space allocation
Results = zeros(4,nfiles);
TPVF = zeros(1,nfiles);
FPVF = zeros(1,nfiles);
PrecisionVal = zeros(1,nfiles);
imgDensity = zeros(1,nfiles);
diceMatrix = zeros(1,nfiles);

for ii=1:nfiles
    
    l1 = imbinarize(testImgs{ii});
    l2 = imbinarize(groundTruths{ii});

    % Default Dice metric
    diceMatrix(ii) = dice(l1,l2);
    
    % Size is based on l1 (both must be the same, or l1 must be smaller)
    [xdim, ydim] = size(l1);
    
    % depends on the definition of foreground/background, now: fg = 0, bg = 1
    trueValue = 0; 
    falseValue = 1;
    
    % FP/TP volumes
    for x = 1:xdim
        for y = 1:ydim
            if ((l1(x,y) == trueValue) && (l2(x,y) == trueValue))
                % True Positive (TP)
                Results(1,ii) = Results(1,ii) + 1;
            elseif ((l1(x,y) == falseValue) && (l2(x,y) == trueValue))
                % False Negative (FN)
                Results(2,ii) = Results(2,ii) + 1;
            elseif ((l1(x,y) == trueValue) && (l2(x,y) == falseValue))
                % False Positive (FP)
                Results(3,ii) = Results(3,ii) + 1;
            elseif ((l1(x,y) == falseValue) && (l2(x,y) == falseValue))
                % True Negative (TN)
                Results(4,ii) = Results(4,ii) + 1;
            else
                "This shouldnt happen"
            end
            
            % Density calculation
            if (l2(x,y) == trueValue)
                imgDensity(ii) = imgDensity(ii) + 1;
            end
        end 
    end
    
    
    imgDensity(ii) = imgDensity(ii) / (xdim * ydim);
    TPVF(ii) = Results(1,ii) / (Results(1,ii) + Results(2,ii));
    FPVF(ii) = Results(3,ii) / (Results(4,ii) + Results(3,ii));
    PrecisionVal(ii) = Results(1,ii) / (Results(3,ii) + Results(1,ii));
    
    
    % if "print" is enabled show both images and false-colour image
    if (print)
        figure, imshow(l1);
        figure, imshow(l2);
        figure, imshowpair(l2,l1, "falsecolor")
        hold on
        t_title = "DSC: " + diceMatrix(ii);
        title(t_title);
        hold off
        
        imgSize = (xdim*ydim);
        fprintf("TP %f  \n", (Results(1,ii)*100)/imgSize);
        fprintf("FP %f  \n", (Results(3,ii)*100)/imgSize);
        fprintf("FN %f  \n", (Results(2,ii)*100)/imgSize);
        fprintf("TN %f  \n", (Results(4,ii)*100)/imgSize);
        
        % KEY
        % Purple -> UNDERESTIMATION (FALSE NEGATIVE)
        % Green -> OVERESTIMATION (FALSE POSITIVE)
        % black -> correct foreground (TRUE POSITIVE)
        % white -> correct background (TRUE NEGATIVE)
        
        %title(['Dice Score = ' num2str(diceMatrix(1,ii)) ', Img Number = ' num2str(ii)])
    end
end

diceValues = diceMatrix;
densityValues = imgDensity;
TPVFVal = TPVF;
FPVFVal = FPVF;

end