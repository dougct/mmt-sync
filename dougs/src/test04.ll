; ModuleID = 'a.out.bc2'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@resultado = global i64 0, align 8

; Function Attrs: nounwind uwtable
define i32 @_Z6divideii(i32 %N, i32 %Q) #0 {
entry:
  call void asm "callq\09pragma_function_begin", ""() #3
  %cmp = icmp sle i32 %Q, 0
  %.Q = select i1 %cmp, i32 2, i32 %Q
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %R.0 = phi i32 [ 0, %entry ], [ %inc, %while.body ]
  %N.addr.0 = phi i32 [ %N, %entry ], [ %sub, %while.body ]
  %cmp1 = icmp sgt i32 %N.addr.0, %.Q
  call void @pragma_dom_begin1() #3
  br i1 %cmp1, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %sub = sub nsw i32 %N.addr.0, %.Q
  %inc = add nsw i32 %R.0, 1
  br label %while.cond

while.end:                                        ; preds = %while.cond
  call void @pragma_dom_end1() #3
  call void asm "callq\09pragma_function_end", ""() #3
  ret i32 %R.0
}

; Function Attrs: uwtable
define i64 @_Z10threadProgi(i32 %threadid) #1 {
entry:
  call void asm "callq\09pragma_function_begin", ""() #3
  %conv = sext i32 %threadid to i64
  %rem = srem i64 %conv, 2
  %tobool = icmp ne i64 %rem, 0
  call void @pragma_dom_begin1() #3
  br i1 %tobool, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %conv1 = trunc i64 10 to i32
  %conv2 = trunc i64 3 to i32
  br label %JointBB

if.then.split:                                    ; preds = %JointBB
  %conv3 = sext i32 %6 to i64
  %inc = add nsw i64 %conv3, 1
  br label %if.end

if.else:                                          ; preds = %entry
  %conv4 = trunc i64 100 to i32
  %conv5 = trunc i64 30 to i32
  br label %JointBB

if.else.split:                                    ; preds = %JointBB
  %conv7 = sext i32 %6 to i64
  %inc8 = add nsw i64 %conv7, 1
  br label %if.end

if.end:                                           ; preds = %if.else.split, %if.then.split
  call void @pragma_dom_end1() #3
  call void @pthread_exit(i8* null) #4
  unreachable

JointBB:                                          ; preds = %if.else, %if.then
  %0 = phi i32 [ undef, %if.then ], [ undef, %if.else ]
  %1 = phi i32 [ undef, %if.then ], [ undef, %if.else ]
  %2 = phi i32 [ undef, %if.else ], [ undef, %if.then ]
  %3 = phi i32 [ undef, %if.else ], [ undef, %if.then ]
  %4 = phi i32 [ %conv1, %if.then ], [ %conv4, %if.else ]
  %5 = phi i32 [ %conv2, %if.then ], [ %conv5, %if.else ]
  %6 = call i32 @_Z6divideii(i32 %4, i32 %5)
  br i1 %tobool, label %if.then.split, label %if.else.split
}

; Function Attrs: noreturn
declare void @pthread_exit(i8*) #2

; Function Attrs: uwtable
define void @_Z12execute_testi(i32 %tid) #1 {
entry:
  call void asm "callq\09pragma_function_end", ""() #3
  call void asm "callq\09pragma_function_begin", ""() #3
  %call = call i64 @_Z10threadProgi(i32 %tid)
  store i64 %call, i64* @resultado, align 8
  ret void
}

; Function Attrs: nounwind uwtable
define void @_Z12prepare_testi(i32 %tid) #0 {
entry:
  call void asm "callq\09pragma_function_end", ""() #3
  call void asm "callq\09pragma_function_begin", ""() #3
  ret void
}

declare void @pragma_dom_begin1()

declare void @pragma_dom_end1()

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noreturn "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }
attributes #4 = { noreturn }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.5 (trunk 201826) (llvm/trunk 201825)"}
