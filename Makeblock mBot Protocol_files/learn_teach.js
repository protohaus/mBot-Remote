"use strict";
    if(document.getElementById('left-nav')!= null){

        window.onload = function(){
//        ———————————侧边栏状态滚动数据初始化

            var leftNav = document.getElementById('left-nav');
//        侧边栏顶部距离
            var leftNavTop = getElementTop(leftNav);
//        侧边栏高度
            var leftNavHeight = leftNav.offsetHeight;
//        页面主题高度
            var detailsHeight = document.getElementsByClassName('teach-mbot-details')[0].offsetHeight;
//        计算页尾顶部距离
            var footer = document.getElementsByClassName('footer')[0];
            var copyright = document.getElementsByClassName('copyright')[0];
            var contentHeight = document.querySelector('.row .content').offsetHeight;
            var footerTop = contentHeight - leftNavHeight+300;
            //var footerTop = getElementTop(footer) - footer.offsetHeight - copyright.offsetHeight -140;
//        ———————————侧边栏状态改变状态初始化

            var navItems = document.querySelectorAll("#left-nav ul li");
            var navData = [];
            for(let i = 0;i<navItems.length;i++){

                if(navItems[i].getAttribute('data-target') != null){
                    let tempName = document.getElementById(navItems[i].getAttribute('data-target'));
                    navData.push({
                        'element': navItems[i],
                        'top': getElementTop(tempName)-20
                    });
                }
            }

            leftNavScroll();
//        滚动监听
            window.addEventListener('scroll',leftNavScroll);
            function leftNavScroll(e){

//            ——————————侧边栏浮动部分
//            判断页面宽度是否大于900、页面可视高度是否大于侧边栏高度、页面滚动距离是否大于侧边栏顶部距离、页面滚动距离是否小于页尾顶部距离
                let scrollTop = document.documentElement.scrollTop || document.body.scrollTop;
                if((document.documentElement.clientWidth > 1200 )&&(document.documentElement.clientHeight > leftNavHeight )&&(scrollTop > leftNavTop)&& (scrollTop < footerTop)){
                    leftNav.style.position = 'fixed';
                    leftNav.style.top = '-30px';
                }else if((document.documentElement.clientWidth > 1000 )&&(document.documentElement.clientHeight > leftNavHeight )&&(scrollTop > footerTop)){
//                侧边栏浮动时，滑倒底部固定侧边栏
                    leftNav.style.position = 'absolute';
                    leftNav.style.top = detailsHeight-leftNavHeight -120 +'px';
                }else{
                    leftNav.style.position = 'relative';
                    leftNav.style.top = '0px';
                }

//            ——————————侧边栏状态改变部分
//            初始化
                for(let i =0;i<navData.length;i++){
                    removeClass(navData[i]['element'],'active');
                }
//            判断侧边栏active状态
                for(let i =0;i<navData.length;i++){
                    if((i!= (navData.length-1))){

                        if((scrollTop > navData[i]['top'])&&(scrollTop < navData[i+1]['top'])){
                            addClass(navData[i]['element'],'active');
                        }
                    }else if(scrollTop > navData[i]['top']){
                        addClass(navData[i]['element'],'active');
                    }

                }
            }
        }

//    计算元素顶部距离
        function getElementTop(element){
            var actualTop = element.offsetTop;
            var current = element.offsetParent;
            while (current !== null){
                actualTop += current.offsetTop;
                current = current.offsetParent;
            }
            return actualTop;
        }

        function addClass(obj, cls){
            var obj_class = obj.className;
            var blank = (obj_class != '') ? ' ' : '';
            var added = obj_class + blank + cls;
            obj.className = added;
        }

        function removeClass(obj, cls){
            var obj_class = ' '+obj.className+' ';
            obj_class = obj_class.replace(/(\s+)/gi, ' ');
            var removed = obj_class.replace(' '+cls+' ', ' ');
            removed = removed.replace(/(^\s+)|(\s+$)/g, '');
            obj.className = removed;//
        }
    }

    // ——————————————————折叠展开

    //接受三个参数，分别是折叠菜单的外包div名称，是否关闭之前的折叠，默认开启的折叠内容
    function Collapse(className,close_prev,default_open){
        this._elements = [];
        this._className = String(className);
        this._previous = Boolean(close_prev);
        this._default = typeof(default_open)==="number" ? default_open: -1
        this.getCurrent;
        this.init();
    }

    //收集所有折叠菜单的div
    Collapse.prototype.collectElementbyClass = function(){
        this._elements = [];
        var allelements = document.getElementsByTagName("div");

        for(var i=0;i<allelements.length;i++){
            var collapse_div = allelements[i];
            if (typeof collapse_div.className === "string" && collapse_div.className === this._className){

                var h3s = collapse_div.getElementsByTagName("h3");
                var collapse_body = collapse_div.getElementsByClassName("collapse_body");
                if(h3s.length === 1 && collapse_body.length === 1){
                    h3s[0].style.cursor = "pointer";

                    if(this._default === this._elements.length){
                        collapse_body[0].style.visibility = "visible";
                        collapse_body[0].style.height = collapse_body[0].scrollHeight+"px"
                    }else{
                        collapse_body[0].style.height = "0px";
                        collapse_body[0].style.visibility = "hidden";
                    }
                    this._elements[this._elements.length] = collapse_div;
                }
            }
        }
    }
    Collapse.prototype.open = function(elm){
        elm.style.visibility = "visible";
        elm.style.height = elm.scrollHeight + "px"

    }
    Collapse.prototype.close = function(elm){
        elm.style.height = "0px";
        elm.style.visibility = "hidden";
    }
    Collapse.prototype.isOpen = function(elm){

        return elm.style.visibility === "visible"
    }

    Collapse.prototype.getCurrent = function(header){
        var cur ;
        if(window.addEventListener){
            cur = header.parentNode
        }else{
            cur = header.parentElement
        }
        return cur.getElementsByClassName("collapse_body")[0]
    }

    Collapse.prototype.toggleDisplay = function(header){

        var cur = this.getCurrent(header)
        //console.log(cur)
        if(this.isOpen(cur)){
            this.close(cur);
        }else{
            this.open(cur);
        }
        if(this._previous){
            for(var i=0;i<this._elements.length;i++){
                if(this._elements[i] !== (cur.parentNode||cur.parentElement)){
                    var collapse_body = this._elements[i].getElementsByClassName("collapse_body");
                    collapse_body[0].style.height = "0px";
                    collapse_body[0].style.visibility = "hidden";

                }
            }
        }
    }

    Collapse.prototype.init = function(){
        var instance = this;
        this.collectElementbyClass();
        if(this._elements.length === 0){
            return;
        }

        for(var i=0;i<this._elements.length;i++){
            var h3s = this._elements[i].getElementsByTagName("h3");
            if(window.addEventListener){
                h3s[0].addEventListener("click",function(){ instance.toggleDisplay(this);},false);
            }else{
                h3s[0].onclick = function(){instance.toggleDisplay(this);}
            }
        }
    }

    //传参
    var myCollapse = new Collapse("collapseDiv",true);
