git pull
git branch newbranch
git checkout newbranch
git add .
git commit -m "`date`"
git checkout master
git merge newbranch
git diff
echo gaojiang@mail.dlut.edu.cn | echo git8494+ |git push -u origin master
git branch -D newbranch
